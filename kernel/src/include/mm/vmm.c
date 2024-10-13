#include "vmm.h"

#define PAGE_ALIGN_DOWN(addr) ((addr / 4096) * 4096)
#define PAGE_ALIGN_UP(x)      ((((x) + 4095) / 4096) * 4096)

#define TOPBITS 0xFFFF000000000000

uint64_t kernel_start       = (uint64_t)p_kernel_start;
uint64_t writeallowed_start = (uint64_t)p_writeallowed_start;
uint64_t kernel_end         = (uint64_t)p_kernel_end;

void ku_memcpy(char* dest, const char* from, size_t n) {
    asm("rep movsb" : : "D"(dest), "S"(from), "c"(n) : "memory");
}

void ku_memset(void* array, uint64_t value, size_t size) {
    asm volatile("rep stosb" : : "D"(array), "a"(value), "c"(size));
}

void map_sections(uint64_t pml4[]) {
    uint64_t                    num_memmap_entries = memmap->entry_count;
    struct limine_memmap_entry* memmap_entries     = *memmap->entries;
    for (size_t entry = 0; entry < num_memmap_entries; entry++) {
        uint64_t entry_type = memmap_entries[entry].type;
        if (entry_type == LIMINE_MEMMAP_USABLE
            || entry_type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE
            || entry_type == LIMINE_MEMMAP_FRAMEBUFFER
            || entry_type == LIMINE_MEMMAP_KERNEL_AND_MODULES
            || entry_type == LIMINE_MEMMAP_ACPI_NVS
            || entry_type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) {
            map_pages(pml4, memmap_entries[entry].base + hhdm_offset,
                      memmap_entries[entry].base,
                      memmap_entries[entry].length / 4096,
                      KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
        }
    }
}

void map_kernel(uint64_t pml4[]) {
    uint64_t length_buffer = 0;
    uint64_t phys_buffer   = 0;
    /* map from kernel_start to writeallowed_start with only the present flag */
    length_buffer = PAGE_ALIGN_UP(writeallowed_start - kernel_start);
    phys_buffer   = kernel_phys_base + (kernel_start - kernel_virt_base);
    map_pages(pml4, PAGE_ALIGN_DOWN(kernel_start), phys_buffer,
              length_buffer / 4096, KERNEL_PFLAG_PRESENT);
    /* map from writeallowed_start to kernel_end with `present` and `write`
     * flags */
    length_buffer = PAGE_ALIGN_UP(kernel_end - writeallowed_start);
    phys_buffer   = kernel_phys_base + (writeallowed_start - kernel_virt_base);
    map_pages(pml4, PAGE_ALIGN_DOWN(writeallowed_start), phys_buffer,
              length_buffer / 4096, KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
    // map the kernel's stack
    alloc_pages(pml4, KERNEL_STACK_ADDR, KERNEL_STACK_PAGES,
                KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE);
}

void map_all(uint64_t pml4[]) {
    map_kernel(pml4);
    map_sections(pml4);
}

uint64_t virt_to_phys(uint64_t pml4_addr[], uint64_t virt_addr) {
    virt_addr     &= ~TOPBITS;
    uint64_t pml1  = (virt_addr >> 12) & 511;
    uint64_t pml2  = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3  = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4  = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t* pml3_addr = NULL;
        if (pml4_addr[pml4] == 0)
            return 0xDEAD;
        else
            pml3_addr =
                (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + hhdm_offset);

        for (; pml3 < 512; pml3++) {
            uint64_t* pml2_addr = NULL;
            if (pml3_addr[pml3] == 0)
                return 0xDEAD;
            else
                pml2_addr =
                    (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + hhdm_offset);

            for (; pml2 < 512; pml2++) {
                uint64_t* pml1_addr = NULL;
                if (pml2_addr[pml2] == 0)
                    return 0xDEAD;
                else
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2])
                                            + hhdm_offset);

                if (pml1_addr[pml1] == 0) return 0xDEAD;

                return (uint64_t)(PAGE_ALIGN_DOWN(pml1_addr[pml1])
                                  + (virt_addr - PAGE_ALIGN_DOWN(virt_addr)));
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    return 0xDEAD;
}

uint64_t phys_to_virt(uint64_t phys_addr) {
    return phys_addr + hhdm_offset;
}

void write_vmem(uint64_t* pml4_addr, uint64_t virt_addr, char* data,
                size_t len) {
    while (len > 0) {
        // get the address of this virtual address in kernel memory
        uint64_t kernel_addr = virt_to_phys(pml4_addr, virt_addr);
        if (kernel_addr == 0xDEAD) {
            asm("hlt");
        }
        kernel_addr            += hhdm_offset;
        uint64_t bytes_to_copy  = (len < PAGE_SIZE) ? len : PAGE_SIZE;
        ku_memcpy((char*)kernel_addr, data, bytes_to_copy);
        len       -= bytes_to_copy;
        virt_addr += bytes_to_copy;
        data      += bytes_to_copy;
    }
}

void push_vmem(uint64_t* pml4_addr, uint64_t rsp, char* data, size_t len) {
    rsp -= len;
    write_vmem(pml4_addr, rsp, data, len);
}

void map_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t phys_addr,
               uint64_t num_pages, uint64_t flags) {
    virt_addr     &= ~TOPBITS;
    uint64_t pml1  = (virt_addr >> 12) & 511;
    uint64_t pml2  = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3  = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4  = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t* pml3_addr = NULL;
        if (pml4_addr[pml4] == 0) {
            pml4_addr[pml4] = (uint64_t)pmm_req_pages(1);
            pml3_addr       = (uint64_t*)(pml4_addr[pml4] + hhdm_offset);
            ku_memset((uint8_t*)pml3_addr, 0, 4096);
            pml4_addr[pml4] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE
                               | KERNEL_PFLAG_USER;
        } else {
            pml3_addr =
                (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + hhdm_offset);
        }

        for (; pml3 < 512; pml3++) {
            uint64_t* pml2_addr = NULL;
            if (pml3_addr[pml3] == 0) {
                pml3_addr[pml3] = (uint64_t)pmm_req_pages(1);
                pml2_addr       = (uint64_t*)(pml3_addr[pml3] + hhdm_offset);
                ku_memset((uint8_t*)pml2_addr, 0, 4096);
                pml3_addr[pml3] |= flags | KERNEL_PFLAG_PRESENT
                                   | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_USER;
            } else {
                pml2_addr =
                    (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + hhdm_offset);
            }

            for (; pml2 < 512; pml2++) {
                uint64_t* pml1_addr = NULL;
                if (pml2_addr[pml2] == 0) {
                    pml2_addr[pml2] = (uint64_t)pmm_req_pages(1);
                    pml1_addr = (uint64_t*)(pml2_addr[pml2] + hhdm_offset);
                    ku_memset((uint8_t*)pml1_addr, 0, 4096);
                    pml2_addr[pml2] |= flags | KERNEL_PFLAG_PRESENT
                                       | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_USER;
                } else {
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2])
                                            + hhdm_offset);
                }
                for (; pml1 < 512; pml1++) {
                    pml1_addr[pml1] = phys_addr | flags;
                    num_pages--;
                    phys_addr += 4096;
                    if (num_pages == 0) return;
                }
                pml1 = 0;
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    printf("\n[KPANIC] Failed to allocate pages: No more avaliable virtual "
           "memory. Halting.\n");
    asm("hlt");
}

void alloc_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t num_pages,
                 uint64_t flags) {
    virt_addr     &= ~TOPBITS;
    uint64_t pml1  = (virt_addr >> 12) & 511;
    uint64_t pml2  = (virt_addr >> (12 + 9)) & 511;
    uint64_t pml3  = (virt_addr >> (12 + 18)) & 511;
    uint64_t pml4  = (virt_addr >> (12 + 27)) & 511;
    for (; pml4 < 512; pml4++) {
        uint64_t* pml3_addr = NULL;
        if (pml4_addr[pml4] == 0) {
            pml4_addr[pml4] = (uint64_t)pmm_req_pages(1);
            pml3_addr       = (uint64_t*)(pml4_addr[pml4] + hhdm_offset);
            ku_memset((uint8_t*)pml3_addr, 0, 4096);
            pml4_addr[pml4] |= flags | KERNEL_PFLAG_PRESENT | KERNEL_PFLAG_WRITE
                               | KERNEL_PFLAG_USER;
        } else {
            pml3_addr =
                (uint64_t*)(PAGE_ALIGN_DOWN(pml4_addr[pml4]) + hhdm_offset);
        }
        for (; pml3 < 512; pml3++) {
            uint64_t* pml2_addr = NULL;
            if (pml3_addr[pml3] == 0) {
                pml3_addr[pml3] = (uint64_t)pmm_req_pages(1);
                pml2_addr       = (uint64_t*)(pml3_addr[pml3] + hhdm_offset);
                ku_memset((uint8_t*)pml2_addr, 0, 4096);
                pml3_addr[pml3] |= flags | KERNEL_PFLAG_PRESENT
                                   | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_USER;
            } else {
                pml2_addr =
                    (uint64_t*)(PAGE_ALIGN_DOWN(pml3_addr[pml3]) + hhdm_offset);
            }

            for (; pml2 < 512; pml2++) {
                uint64_t* pml1_addr = NULL;
                if (pml2_addr[pml2] == 0) {
                    pml2_addr[pml2] = (uint64_t)pmm_req_pages(1);
                    pml1_addr = (uint64_t*)(pml2_addr[pml2] + hhdm_offset);
                    ku_memset((uint8_t*)pml1_addr, 0, 4096);
                    pml2_addr[pml2] |= flags | KERNEL_PFLAG_PRESENT
                                       | KERNEL_PFLAG_WRITE | KERNEL_PFLAG_USER;
                } else {
                    pml1_addr = (uint64_t*)(PAGE_ALIGN_DOWN(pml2_addr[pml2])
                                            + hhdm_offset);
                }

                for (; pml1 < 512; pml1++) {
                    uint64_t phys = (uint64_t)pmm_req_pages(1);
                    ku_memset((uint8_t*)(phys + hhdm_offset), 0, PAGE_SIZE);
                    pml1_addr[pml1] = phys | flags;
                    num_pages--;
                    if (num_pages == 0) return;
                }
                pml1 = 0;
            }
            pml2 = 0;
        }
        pml3 = 0;
    }
    printf("Failed to allocate pages: No more avaliable virtual memory. "
           "Halting.\n");
    asm("hlt");
}

void clear_page_cache(uint64_t addr) {
    asm volatile("invlpg (%0)" ::"r"(addr) : "memory");
}

uint64_t* init_paging_task() {
    uint64_t pml4_virt = ((uint64_t)pmm_req_pages(1)) + hhdm_offset;
    ku_memset((uint8_t*)pml4_virt, 0, 4096);
    map_all((uint64_t*)pml4_virt);
    return (uint64_t*)pml4_virt;
}

uint64_t init_vmm() {
    uint64_t pml4_virt = ((uint64_t)pmm_req_pages(1)) + hhdm_offset;
    ku_memset((uint8_t*)pml4_virt, 0, 4096);
    map_all((uint64_t*)pml4_virt);
    uint64_t cr3 = pml4_virt - hhdm_offset;
    
    printf("Successfully initialized VMM!\n");
    return cr3;
}
