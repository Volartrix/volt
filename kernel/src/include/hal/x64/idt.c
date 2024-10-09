#include "idt.h"
#include "syscalls.h"

idtEntry_t      idtEntries[IDT_ENTRY_COUNT];
idtPointer_t    idtPointer;
extern uint64_t isrTable[];

extern void iretq_asm(void);

irqHandler_t irqHandlers[IRQ_COUNT] = {0};

static const char* exceptionStrings[32] = {"Division By Zero",
                                           "Debug",
                                           "Nonmaskable Interrupt",
                                           "Breakpoint",
                                           "Overflow",
                                           "Bound Range Exceeded",
                                           "Invalid Opcode",
                                           "Device Not Available",
                                           "Double Fault",
                                           "Coprocessor Segment Overrun",
                                           "Invalid TSS",
                                           "Segment Not Present",
                                           "Stack Segment Fault",
                                           "General Protection Fault",
                                           "Page Fault",
                                           "Reserved",
                                           "x87 FPU Error",
                                           "Alignment Check",
                                           "Machine Check",
                                           "Simd Exception",
                                           "Virtualization Exception",
                                           "Control Protection Exception",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Hypervisor Injection Exception",
                                           "VMM Communication Exception",
                                           "Security Exception",
                                           "Reserved"};


void idt_set_gate(idtEntry_t idt[], int num, uint64_t base, uint16_t sel,
                  uint8_t flags) {
    idt[num].offsetLow    = (base & 0xFFFF);
    idt[num].offsetMiddle = (base >> 16) & 0xFFFF;
    idt[num].offsetHigh   = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector     = sel;
    idt[num].ist          = 0;
    idt[num].flags        = flags;
    idt[num].zero         = 0;
}

void idt_init() {
    idtPointer.limit = sizeof(idtEntry_t) * IDT_ENTRY_COUNT - 1;
    idtPointer.base  = (uintptr_t)&idtEntries;

    for (size_t i = 0; i < IRQ_COUNT; i++) {
        irqHandlers[i] = NULL;
    }

    for (int i = 0; i < 32; ++i) {
        idt_set_gate(idtEntries, i, isrTable[i], 0x08, 0x8E);
    }

    for (int i = IRQ_BASE; i < IRQ_BASE + IRQ_COUNT; ++i) {
        idt_set_gate(idtEntries, i, isrTable[i], 0x08, 0x8E);
    }

    idt_set_gate(idtEntries, 0x80, (uint64_t)isrTable[0x80], 0x08, 0xEE);

    idt_load((uint64_t)&idtPointer);

    printf("Initialized IDT: \n\tLimit: 0x%.3llX\n\tBase: 0x%.16llX\n");
}

void IdtExcpHandler(Context_t frame) {
    if (frame.vector < 0x20) {
        cls();
        printf("#################################################\n");
        printf("# !!! A CPU EXCEPTION INTERRUPTED EXECUTION !!! #\n");
        printf("#################################################\n");
        printf("Type: %s\n", exceptionStrings[frame.vector]);
        printf("Error Code: %.16llX\n\n", frame.err);
        printf("# REGS #\n\n");
        printf("RAX: %.16llX\n", frame.rax);
        printf("RIP: %.16llX\n", frame.rip);

        asm("hlt");
    } else if (frame.vector == 0x80) {
        handle_syscall(frame);
        iretq_asm();
    }
}
