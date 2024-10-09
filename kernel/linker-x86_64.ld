/* Tell the linker that we want an x86_64 ELF64 output file */
OUTPUT_FORMAT(elf64-x86-64)

/* We want the symbol kmain to be our entry point */
ENTRY(kmain)

/* Define the program headers we want so the bootloader gives us the right */
/* MMU permissions; this also allows us to exert more control over the linking */
/* process. */
PHDRS
{
    requests PT_LOAD;
    text     PT_LOAD;
    rodata   PT_LOAD;
    data     PT_LOAD;
}

SECTIONS
{
    . = 0xffffffff80000000;
    p_kernel_start = .;

    .requests : {
        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    } :requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .text : {
        *(.text .text.*)
    } :text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata .rodata.*)
    } :rodata

    /* Move to the next memory page for .data */
    . = ALIGN(CONSTANT(MAXPAGESIZE));

    p_writeallowed_start = .;

    .data : {
        *(.data .data.*)
    } :data

    .dynamic : {
        *(.dynamic)
    } :data :dynamic

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } :data

    p_kernel_end = .;

    /* Discard .note.* and .eh_frame* since they may cause issues on some hosts. */
    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
