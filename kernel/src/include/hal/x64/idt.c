#include "idt.h"
#include "syscalls.h"

idtEntry_t      idtEntries[IDT_ENTRY_COUNT];
idtPointer_t    idtPointer;
extern uint64_t isrTable[];


struct stackFrame {
    struct stackFrame* rbp;
    uint64_t           rip;
};

void stack_trace(uint64_t rbp, uint64_t rip) {
    printf("\nMost recent call last: \n");
    printf(" 0x%016llX\n", rip);
    struct stackFrame* stack = (struct stackFrame*)rbp;
    while (stack) {
        printf(" 0x%016llX\n", stack->rip);
        stack = stack->rbp;
    }
}


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
        char* rflags_string = rflags_str(frame.rflags);
        if (rflags_string == NULL) {
            printf("What the fuck?????");
            rflags_string = "Fucking";
        }
        cls();
        printf("####################################\n");
        printf("# !!! A CPU EXCEPTION OCCURRED !!! #\n");
        printf("####################################\n");
        printf("Type: %s\n", exceptionStrings[frame.vector]);
        printf("Error Code: 0b%s\n\n", uint32_to_bin((uint32_t)frame.err));
        printf("+------+\n");
        printf("| REGS |\n");
        printf("+------+\n\n");
        printf(
            "RAX: 0x%.16llX, RBX: 0x%.16llX, RCX: 0x%.16llX, RDX: 0x%.16llX\n",
            frame.rax, frame.rbx, frame.rcx, frame.rdx);
        printf(
            "RSI: 0x%.16llX, RDI: 0x%.16llX, RBP: 0x%.16llX, RSP: 0x%.16llX\n",
            frame.rsi, frame.rdi, frame.rbp, frame.rsp);
        printf(
            "R8:  0x%.16llX, R9:  0x%.16llX, R10: 0x%.16llX, R11: 0x%.16llX\n",
            frame.r8, frame.r9, frame.r10, frame.r11);
        printf(
            "R12: 0x%.16llX, R13: 0x%.16llX, R14: 0x%.16llX, R15: 0x%.16llX\n",
            frame.r12, frame.r13, frame.r14, frame.r15);
        printf("RIP: 0x%.16llX, RFL: 0x%.8llX [%s] %s\n", frame.rip,
               frame.rflags, rflags_string,
               get_rfl_other(frame.rflags, frame.cs));
        printf(segment_str(frame.cs, frame.ss, frame.ds));
        printf(get_gdt_idt());
        printf(get_control_registers());
        printf("\n\n+------------+\n");
        printf("| STACKTRACE |\n");
        printf("+------------+\n");
        stack_trace(frame.rbp, frame.rip);

        printf("\n\n+-------------------------+\n");
        printf("| PRESS ESC KEY TO REBOOT |\n");
        printf("+-------------------------+\n");

        char c = 0;

        do {

            if (inb(0x60) != c)    // PORT FROM WHICH WE READ
            {
                c = inb(0x60);
                if (c > 0) {

                    printf(".");
                }
            }


        } while (c != 1);    // 1= ESCAPE

        uint8_t good = 0x02;
        while (good & 0x02) good = inb(0x64);
        outb(0x64, 0xFE);
        asm("hlt");

    } else {
        asm("iretq");
    }
}
