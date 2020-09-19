// -*- c++ -*-

#include "Config.h"

#include "DS4USB.h"
#include "Host.h"

Host host;
USB usb;
DS4USB ds4(&usb, &host);

void setup()
{
  initLog();

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  delay(5000);
#endif

  if (usb.Init() == -1) {
    LogError(USB_INIT_FAILED);
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  host.loop();
  usb.Task();
}
