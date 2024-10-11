#include <flanterm/backends/fb.h>
#include <flanterm/flanterm.h>
#include <hal/x64/gdt.h>
#include <hal/x64/idt.h>
#include <limine.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

extern uint64_t p_kernel_start[];
extern uint64_t p_writeallowed_start[];
extern uint64_t p_kernel_end[];

struct flanterm_context* ft_ctx;

__attribute__((used,
               section(".requests"))) static volatile LIMINE_BASE_REVISION(2);

__attribute__((
    used,
    section(".requests"))) static volatile struct limine_framebuffer_request
    framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

__attribute__((
    used, section(".requests"))) static volatile struct limine_memmap_request
    memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};

__attribute__((used,
               section(".requests"))) static volatile struct limine_hhdm_request
    hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};

__attribute__((
    used,
    section(".requests"))) static volatile struct limine_kernel_address_request
    kernel_addr_request = {.id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

__attribute__((used,
               section(".requests_start_"
                       "marker"))) static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((
    used,
    section(
        ".requests_end_marker"))) static volatile LIMINE_REQUESTS_END_MARKER;

uint64_t                               hhdm_offset      = 0;
uint64_t                               hddm_size        = 0;
struct limine_memmap_response*         memmap           = NULL;
struct limine_kernel_address_response* kernel_addr      = NULL;
uint64_t                               kernel_phys_base = 0;
uint64_t                               kernel_virt_base = 0;


static void hcf(void) {
    for (;;) {
        asm("hlt");
    }
}

uint64_t* pml4_global = NULL;

void kmain(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (framebuffer_request.response == NULL
        || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    struct limine_framebuffer* framebuffer =
        framebuffer_request.response->framebuffers[0];

    ft_ctx = flanterm_fb_init(
        NULL, NULL, framebuffer->address, framebuffer->width,
        framebuffer->height, framebuffer->pitch, framebuffer->red_mask_size,
        framebuffer->red_mask_shift, framebuffer->green_mask_size,
        framebuffer->green_mask_shift, framebuffer->blue_mask_size,
        framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, 0, 0, 1, 0, 0, 0);

    hhdm_offset      = hhdm_request.response->offset;
    memmap           = memmap_request.response;
    kernel_addr      = kernel_addr_request.response;
    kernel_phys_base = kernel_addr->physical_base;
    kernel_virt_base = kernel_addr->virtual_base;

    ft_ctx->cursor_enabled = false;
    ft_ctx->tab_size       = 2;

    gdt_init();
    idt_init();

    pmm_init();

    uint64_t free_memory = pmm_get_free();

    printf("Free memory: %dMB bytes\n\n", free_memory / 1024 / 1024);

    printf("Kernel Start: %.16llX\n", p_kernel_start);
    printf("Write Allowed Start: %.16llX\n", p_writeallowed_start);
    printf("Kernel End: %.16llX\n\n", p_kernel_end);

    asm("int $0x3");

    hcf();
}
