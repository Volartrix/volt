#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "idt.h"
#include <stdio.h>

// so yea ill use return codes for one if something is fucked up
void handle_syscall(Context_t ctx);

#endif    // __SYSCALL_H__
