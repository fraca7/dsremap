
#ifdef TARGET_PC
#include <stdlib.h>
#include <stdint.h>
#else
#include "Config.h"
#endif

#include "Configuration.h"
#include "IMUIntegrator.h"
#include "VM.h"

void Configuration::Run(USBReport01_t* report, const IMUIntegrator* imu)
{
  for (uint8_t i = 0; i < Count(); ++i)
    GetItem(i)->Run(report, imu);
}
