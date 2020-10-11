
#ifndef _VM_C_H
#define _VM_C_H

#include <stdint.h>

#include "cpp/DS4Structs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct imu_t;
struct vm_t;

struct vm_t* vm_init(const uint8_t* bytecode, size_t bytecode_len, uint8_t stack_size);
void         vm_free(struct vm_t* vm);
void         vm_run(struct vm_t* vm, BTReport11_t* report, const struct imu_t* imu);

#ifdef __cplusplus
}
#endif

#endif /* _VM_C_H */
