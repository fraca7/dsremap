
#include <glib.h>
#include <fcntl.h>
#include <unistd.h>

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

struct vm_t* vm_init_from_file(const char* filename)
{
  vm_t* vm = NULL;
  uint8_t* bytecode;

  int fd = open(filename, O_RDONLY);
  if (fd < 0)
    return NULL;

  struct {
    uint16_t magic;
    uint16_t conflen;
    uint16_t actionlen;
    uint16_t stacksize;
  } header;

  if (read(fd, &header, sizeof(header)) != sizeof(header))
    goto fail_1;
  if (header.magic != 0xCAFE)
    goto fail_1;

  bytecode = (uint8_t*)g_malloc(header.actionlen - 2);
  if (read(fd, bytecode, header.actionlen - 2) != header.actionlen - 2)
    goto fail_2;

  vm = vm_init(bytecode, header.actionlen - 2, header.stacksize);

 fail_2:
  g_free(bytecode);

 fail_1:
  close(fd);

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
