#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <stdio.h>
#include "idt.h"

// so yea ill use return codes for one if something is fucked up
void handle_syscall(Context_t ctx);

#endif // __SYSCALL_H__