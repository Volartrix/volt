#include "syscalls.h"
#include <hal/x64/idt.h>

void handle_syscall(Context_t ctx) {

    printf("Test syscalls\n");
}
