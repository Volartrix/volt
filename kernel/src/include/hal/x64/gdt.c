#include "gdt.h"

gdtPointer_t gdtPointer;
gdtEntry_t   gdtEntries[5];


void gdt_init() {
    gdtEntries[0] = (gdtEntry_t){0, 0, 0, 0, 0, 0};
    gdtEntries[1] = (gdtEntry_t){0, 0, 0, 0b10011010, 0b10100000, 0};
    gdtEntries[2] = (gdtEntry_t){0, 0, 0, 0b10010010, 0b10100000, 0};
    gdtEntries[3] = (gdtEntry_t){0, 0, 0, 0b11111010, 0b10100000, 0};
    gdtEntries[4] = (gdtEntry_t){0, 0, 0, 0b11110010, 0b10100000, 0};

    gdtPointer.limit = (uint16_t)(sizeof(gdtEntries) - 1);
    gdtPointer.base  = (uintptr_t)&gdtEntries;

    gdt_flush(gdtPointer);
    printf("Initialized GDT: \n\tLimit: 0x%.3llX \n\tBase: 0x%.16llX\n", gdtPointer.limit, gdtPointer.base);
}

void gdt_flush(gdtPointer_t gdtr) {
    __asm__ volatile("mov %0, %%rdi\n"
                     "lgdt (%%rdi)\n"
                     "push $0x8\n"
                     "lea 1f(%%rip), %%rax\n"
                     "push %%rax\n"
                     "lretq\n"
                     "1:\n"
                     "mov $0x10, %%ax\n"
                     "mov %%ax, %%es\n"
                     "mov %%ax, %%ss\n"
                     "mov %%ax, %%gs\n"
                     "mov %%ax, %%ds\n"
                     "mov %%ax, %%fs\n"
                     :
                     : "r"(&gdtr)
                     : "memory");
}
