
#include <glib.h>

#include "vm.h"
#include "imu.h"
#include "imu_private.h"

#include "cpp/VM.h"

struct vm_t {
  VM* ptr;
  uint8_t* bytecode;
};

struct vm_t* vm_init(const uint8_t* bytecode, size_t bytecode_len, uint8_t stack_size)
{
  struct vm_t* vm = (struct vm_t*)g_malloc(sizeof(struct vm_t));
  vm->bytecode = (uint8_t*)g_malloc(bytecode_len);
  memcpy(vm->bytecode, bytecode, bytecode_len);
  vm->ptr = new VM(vm->bytecode, false, stack_size);
  return vm;
}

void vm_free(struct vm_t* vm)
{
  delete vm->ptr;
  g_free(vm->bytecode);
  g_free(vm);
}

void vm_run(struct vm_t* vm, BTReport11_t* report, const struct imu_t* imu)
{
  vm->ptr->Run((USBReport01_t*)((uint8_t*)report + 2), imu->ptr);
}
