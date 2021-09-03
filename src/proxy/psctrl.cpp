/**
 * @file dualsense.cpp
 */

/**********************************************************************

  Created: 01 Sep 2021

    Copyright (C) 2021 Quividi

**********************************************************************/

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include <unistd.h>

#include <src/utils/Application.h>
#include <src/utils/Logger.h>
#include <src/usb/Dualsense.h>
#include <src/usb/Dualshock4.h>

using namespace std;
using namespace dsremap;

class App : public Logger,
            public Application,
            public USBDevice::Listener,
            public HIDInterface::Listener
{
public:
  enum class ControllerType {
    Dualshock,
    Dualsense
  };

  App(ControllerType type, const nlohmann::json& reports)
    : Logger("dualsense"),
      _ctrl((type == ControllerType::Dualshock) ? (USBDevice*)(new Dualshock4(*this, *this)) : (USBDevice*)(new Dualsense(*this, *this))),
      _reports()
  {
    for (const auto& kv : reports.items()) {
      int report_id = stoi(kv.key(), nullptr, 16);
      string bytes = kv.value().get<string>();
      vector<uint8_t> report;
      for (unsigned int i = 0; i < bytes.size(); i += 2) {
        int v;
        sscanf(bytes.c_str() + i, "%02x", &v);
        report.push_back(v);
      }

      cerr << "Registered report " << report_id << "; len=" << report.size() << endl;
      _reports.insert(make_pair(report_id, report));
    }
  }

  void run() {
    _ctrl->attach();
    Application::run();
  }

  void on_error(exception_ptr eptr) override {
    try {
      rethrow_exception(eptr);
    } catch (const exception& exc) {
      cerr << exc.what() << endl;
    }

    stop();
  }

  void on_device_attached(USBDevice&) override {
    cerr << "!! Attached" << endl;
  }

  void on_device_detached(USBDevice&) override {
    cerr << "!! Detached" << endl;
  }

  void on_usb_get_report(ControlEndpoint& ep, int type, int id) override {
    info("GET_REPORT type=0x{:02x}, id=0x{:02x}", type, id);

    auto pos = _reports.find(id);
    if (pos == _reports.end()) {
      warn("Unhandled GET_REPORT");
    } else {
      ep.write(pos->second.data(), pos->second.size());
    }
  }

  void on_usb_set_report(int type, int id, const vector<uint8_t>& data) override {
    info("SET_REPORT type=0x{:02x}, id=0x{:02x}", type, id);
    debug_hex(data.data(), data.size());
  }

  void on_usb_out_report(int id, const vector<uint8_t>& data) override {
    info("OUT report id=0x{:02x}", id);
    debug_hex(data.data(), data.size());
  }

private:
  shared_ptr<USBDevice> _ctrl;
  map<int, vector<uint8_t>> _reports;
};

int main(int argc, char* argv[]) {
  if (geteuid() != 0) {
    cerr << "Must be run as root." << endl;
    return 1;
  }

  if (argc != 2) {
    cerr << "Usage: psctrl <json file>" << endl;
    return 1;
  }

  nlohmann::json reports;
  {
    ifstream ifs(argv[1]);
    ifs >> reports;
  }

  App::ControllerType type = App::ControllerType::Dualshock;
  uint16_t pid = reports["pid"].get<int>();
  switch (pid) {
    case 0x09cc:
      break;
    case 0x0ce6:
      type = App::ControllerType::Dualsense;
      break;
    default:
      cerr << "Unknown PID " << pid << endl;
      return 1;
  }

  App app(type, reports["reports"]);
  app.run();

  return 0;
}
