#include "regs.h"

char* rflags_str(uint64_t rflags) {
    static char result[9] = "--------";

    if (rflags & (1 << 0)) result[7] = 'C';
    if (rflags & (1 << 2)) result[6] = 'P';
    if (rflags & (1 << 4)) result[5] = 'A';
    if (rflags & (1 << 6)) result[4] = 'Z';
    if (rflags & (1 << 7)) result[3] = 'S';
    if (rflags & (1 << 8)) result[2] = 'T';
    if (rflags & (1 << 9)) result[1] = 'I';
    if (rflags & (1 << 11)) result[0] = 'O';

    return result;
}

// fuck idk what to call this. Well it return something like this = "CPL=0 II=0
// A20=1"
char* get_rfl_other(uint64_t rfl, uint64_t cs_sel) {
    static char result[20];

    uint64_t ii  = (rfl & (1 << 9)) ? 1 : 0;
    int      cpl = cs_sel & 0x3;

    unsigned char a20_res;
    asm volatile("inb $0x64, %0" : "=a"(a20_res));

    int a20 = (a20_res & 0x02) ? 0 : 1;

    snprintf(result, sizeof(result), "CPL=%d II=%d A20=%d", cpl, ii, a20);

    return result;
}

int get_segment_dpl(uint64_t segment) {
    return (segment >> 13) & 0x3;
}

const char* get_segment_type(uint64_t segment) {
    if (segment & (1 << 11)) {       // Code/Data segments
        if (segment & (1 << 3)) {    // Executable (code segment)
            return (segment & (1 << 9)) ? "CS64 [-R-]" : "CS64 [-WA]";
        } else {                     // Data segment
            return (segment & (1 << 9)) ? "DS   [-R-]" : "DS   [-WA]";
        }
    }
    return "DS   [-WA]";    // Default for non-system segments
}

char* segment_str(uint64_t cs, uint64_t ss, uint64_t ds) {
    static char result[512];    // Preallocate space for the result string

    // Build the result string with formatted output for CS, SS, and DS segment
    // descriptors
    snprintf(result, sizeof(result),
             "CS  = 0x%04lX\n"
             "SS  = 0x%04lX\n"
             "DS  = 0x%04lX\n",
             cs, ss, ds);

    return result;
}

char* get_gdt_idt() {
    static char  result[256];    // Preallocate space for the result string
    descriptor_t gdt, idt;

    // Use inline assembly to get GDT
    __asm__ __volatile__("sgdt %0" : "=m"(gdt));
    // Use inline assembly to get IDT
    __asm__ __volatile__("sidt %0" : "=m"(idt));

    // Format the result string
    snprintf(result, sizeof(result),
             "GDT = 0x%016lX %08x\n"
             "IDT = 0x%016lX %08x\n",
             gdt.base, gdt.limit, idt.base, idt.limit);

    return result;
}

char* get_control_registers() {
    static char result[128];    // Preallocate space for the result string
    uint64_t    cr0, cr2, cr3, cr4;

    // Use inline assembly to get control registers
    __asm__ __volatile__("mov %%cr0, %0\n"
                         "mov %%cr2, %1\n"
                         "mov %%cr3, %2\n"
                         "mov %%cr4, %3\n"
                         : "=r"(cr0), "=r"(cr2), "=r"(cr3), "=r"(cr4));

    // Format the result string
    snprintf(result, sizeof(result),
             "CR0 = 0x%.8lX CR2 = 0x%.8lX CR3 = 0x%.8lX CR4 = 0x%.8lX", cr0,
             cr2, cr3, cr4);

    return result;
}
