
#include "DS4USB.h"

#include "Host.h"

DS4USB::DS4USB(USB* pUsb, Host* pHost)
  : HIDUniversal(pUsb),
    m_pHost(pHost),
    m_Interface(0xFF),
    m_PendingCalib(false)
{
}

uint8_t DS4USB::OnInitSuccessful()
{
  LogInfo(DEVICE_PLUGGED);

  // SET_INTERFACE on audio interfaces 1 & 2
  pUsb->ctrlReq(bAddress, 0x00, 0b00000001, 0x0b, 0x00, 0x01, 0x0100, 0x0000, 0, NULL, NULL);
  pUsb->ctrlReq(bAddress, 0x00, 0b00000001, 0x0b, 0x00, 0x01, 0x0200, 0x0000, 0, NULL, NULL);

  uint8_t rcode = SetIdle(m_Interface, 0, 0);
  if (rcode)
    LogError(ERROR_SET_IDLE, rcode);

  // Get gyro/accell calibration data before starting.
  m_PendingCalib = true;
  GET_REPORT(0x03, 0x02, 37);
  m_PendingCalib = false;

  m_pHost->OnDeviceReady(this);

  return 0;
}

void DS4USB::ParseHIDData(USBHID* pHID, bool is_rpt_id, uint8_t len, uint8_t* buf)
{
  if ((buf[0] != 0x01) || (len != 0x40)) {
    LogWarning(WARN_OOB_REPORT, buf[0]);
    LogDump(len, buf);
  }

  m_pHost->OnDeviceData(len, buf);
}

void DS4USB::EndpointXtract(uint8_t conf, uint8_t iface, uint8_t alt, uint8_t proto, const USB_ENDPOINT_DESCRIPTOR *pep)
{
  if (m_Interface == 0xFF) {
    m_Interface = iface;
    LogDebug(DS4_HID_IF, iface);
  }

  HIDUniversal::EndpointXtract(conf, iface, alt, proto, pep);
}

void DS4USB::GET_REPORT(uint8_t reportType, uint8_t reportId, uint16_t len)
{
  uint8_t reportBuf[64];

  // Seems to happen from time to time, regular 0x01 reports appearing
  // on the control endpoint, and control reports on the IN endpoint...

  while (true) {
    uint8_t rcode = GetReport(0, m_Interface, reportType, reportId, len, reportBuf);
    if (rcode) {
      LogError(ERROR_GET_REPORT, rcode);
      return;
    }

    if (reportBuf[0] == reportId)
      break;
  }

  if (m_PendingCalib)
    m_pHost->SetCalibrationData((CalibrationData_t*)&reportBuf[1]);
  else
    m_pHost->SendReport(len, reportBuf);
}

void DS4USB::SET_REPORT(uint8_t reportType, uint8_t reportId, uint16_t len, const uint8_t* buf)
{
  uint8_t rcode = SetReport(0, m_Interface, reportType, reportId, len, const_cast<uint8_t*>(buf));
  if (rcode)
    LogError(ERROR_SET_REPORT, rcode);
}

void DS4USB::SendData(uint16_t len, const uint8_t* buf)
{
  uint8_t rcode = SndRpt(len, const_cast<uint8_t*>(buf));
  if (rcode)
    LogError(ERROR_SNDRPT, rcode);
}
