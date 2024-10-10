#ifndef __REGS_H__
#define __REGS_H__

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint16_t limit;
    uint64_t base;
} descriptor_t;

// Kind of like the QEMU string the "RFL=00000096 [--S-AP-]" one
char*       rflags_str(uint64_t rflags);
char*       get_rfl_other(uint64_t rfl, uint64_t cs_sel);
int         get_segment_dpl(uint64_t segment);
const char* get_segment_type(uint64_t segment);
char*       segment_str(uint64_t cs, uint64_t ss, uint64_t ds);
char*       get_gdt_idt();
char*       get_control_registers();
#endif    // __REGS_H__
