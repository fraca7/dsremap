// -*- c++ -*-

#include "Config.h"

#include "Host.h"

#if TARGET_PLATFORM == TARGET_PS4
#include "DS4USB.h"
#elif TARGET_PLATFORM == TARGET_PS5
#include "DS5USB.h"
#endif

Host host;
USB usb;
#if TARGET_PLATFORM == TARGET_PS4
DS4USB ctrl(&usb, &host);
#elif TARGET_PLATFORM == TARGET_PS5
DS5USB ctrl(&usb, &host);
#endif

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
