/**
 * @file HTTPServer.cpp
 */

/**********************************************************************

  Created: 13 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <fstream>

#include <nlohmann/json.hpp>
#include <stdlib.h>

#include <src/utils/Application.h>
#include <src/utils/format.h>
#include <src/bluetooth/BTUtils.h>

#include "HTTPServer.h"

namespace dsremap
{
  HTTPServer::HTTPServer(Application& app, unsigned int port, const std::string& config_path)
    : Logger("HTTP"),
      _app(app),
      _server(soup_server_new(SOUP_SERVER_SERVER_HEADER, "dsremap-server", NULL)),
      _config_path(config_path)
  {
    GError *err = NULL;
    if (!soup_server_listen_all(_server, port, (SoupServerListenOptions)0, &err)) {
      std::string msg = format("Cannot listen on port {}: {}", port, err->message);
      g_object_unref(_server);
      g_error_free(err);
      throw std::runtime_error(msg);
    }

    soup_server_add_handler(_server, "/info", &HTTPServer::static_request_callback, static_cast<gpointer>(this), NULL);
    soup_server_add_handler(_server, "/set_config", &HTTPServer::static_request_callback, static_cast<gpointer>(this), NULL);
    soup_server_add_handler(_server, "/reboot", &HTTPServer::static_request_callback, static_cast<gpointer>(this), NULL);
    soup_server_add_handler(_server, "/halt", &HTTPServer::static_request_callback, static_cast<gpointer>(this), NULL);
  }

  HTTPServer::~HTTPServer()
  {
    g_object_unref(_server);
  }

  void HTTPServer::static_request_callback(SoupServer*, SoupMessage* msg, const char* path, GHashTable* query, SoupClientContext* client, gpointer ptr)
  {
    static_cast<HTTPServer*>(ptr)->request_callback(msg, path, query, client);
  }

  void HTTPServer::request_callback(SoupMessage* msg, const char* path, GHashTable*, SoupClientContext*)
  {
    info("Request for \"{}\"", path);

    if (!strcmp(path, "/info")) {
      if (msg->method != SOUP_METHOD_GET) {
        warn("Invalid method");
        soup_message_set_status(msg, SOUP_STATUS_METHOD_NOT_ALLOWED);
        return;
      }

      try {
        HCIDevice dev(0);
        uint8_t addr[6];
        dev.get_bdaddr((bdaddr_t*)addr);

        soup_message_set_status(msg, SOUP_STATUS_OK);
        std::string bdaddr = format("{:02x}:{:02x}:{:02x}:{:02x}:{:02x}:{:02x}", addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
        nlohmann::json response;
        response["bdaddr"] = bdaddr;
        response["version"] = format("{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

        std::string str_resp = response.dump(2);
        soup_message_set_response(msg, "application/json", SOUP_MEMORY_COPY, str_resp.c_str(), str_resp.size());
      } catch (const std::exception& exc) {
        error("Error handling info: {}", exc.what());
        soup_message_set_status_full(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, exc.what());
      }
    } else if (!strcmp(path, "/set_config")) {
      if (msg->method != SOUP_METHOD_POST) { // Should be PUT really but whatever
        warn("Invalid method");
        soup_message_set_status(msg, SOUP_STATUS_METHOD_NOT_ALLOWED);
        return;
      }

      SoupBuffer* buffer = soup_message_body_flatten(msg->request_body);
      const guint8* data;
      gsize len;
      soup_buffer_get_data(buffer, &data, &len);
      {
        std::ofstream ofs(_config_path, std::ios::trunc);
        ofs.write((const char*)data, len);
      }
      soup_buffer_free(buffer);

      info("Reconfiguring");
      try {
        _app.reconfigure();
        soup_message_set_status(msg, SOUP_STATUS_OK);
      } catch (const std::exception& exc) {
        error("While reconfiguring: {}", exc.what());
        soup_message_set_status_full(msg, SOUP_STATUS_INTERNAL_SERVER_ERROR, exc.what());
      }
    } else if (!strcmp(path, "/reboot")) {
      info("Rebooting");
      soup_message_set_status(msg, SOUP_STATUS_OK);
      system("reboot");
    } else if (!strcmp(path, "/halt")) {
      info("Halting");
      soup_message_set_status(msg, SOUP_STATUS_OK);
      system("halt -p");
    } else {
      soup_message_set_status(msg, SOUP_STATUS_NOT_FOUND);
    }
  }
}
