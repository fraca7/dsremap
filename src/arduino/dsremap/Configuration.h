
#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include "darray.h"
#include "DS4Structs.h"

#include "VM.h"

class IMUIntegrator;

class Configuration : public DArray<VM>
{
public:
  using DArray<VM>::DArray;

  void Run(controller_state_t*, const IMUIntegrator*);
};

#endif /* _CONFIGURATION_H */
