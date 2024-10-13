#ifndef __VMM_H__
#define __VMM_H__

#include <mm/pmm.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern struct limine_memmap_response* memmap;
extern uint64_t                       kernel_phys_base;
extern uint64_t                       kernel_virt_base;

extern uint64_t p_kernel_start[];
extern uint64_t p_writeallowed_start[];
extern uint64_t p_kernel_end[];

extern uint64_t* pml4_global;

// thx UnmappedStack
#define KERNEL_PFLAG_PRESENT 0b1
#define KERNEL_PFLAG_WRITE   0b10
#define KERNEL_PFLAG_USER    0b100
#define KERNEL_PFLAG_PXD \
    0b10000000000000000000000000000000000000000000000000000000000000    // a bit
                                                                        // long
                                                                        // lmao

#define PAGE_SIZE          4096
#define KERNEL_STACK_PAGES 2LL
#define KERNEL_STACK_PTR   0xFFFFFFFFFFFFF000LL
#define KERNEL_STACK_ADDR  KERNEL_STACK_PTR - (KERNEL_STACK_PAGES * PAGE_SIZE)

#define USER_STACK_PAGES 2LL
#define USER_STACK_ADDR  (USER_STACK_PTR - USER_STACK_PAGES * PAGE_SIZE)
#define USER_STACK_PTR   0x700000000000LL

#define PAGE_ALIGN_DOWN(addr) \
    ((addr / 4096) * 4096)    // works cos of integer division
#define PAGE_ALIGN_UP(x) ((((x) + 4095) / 4096) * 4096)

#define KERNEL_SWITCH_PAGE_TREE(TREE_ADDRESS) \
    __asm__ volatile("movq %0, %%cr3" : : "r"(TREE_ADDRESS))

#define KERNEL_SWITCH_STACK()           \
    __asm__ volatile("movq %0, %%rsp\n" \
                     "movq $0, %%rbp\n" \
                     "push $0"          \
                     :                  \
                     : "r"(KERNEL_STACK_PTR))

uint64_t  init_vmm(uint64_t* pml4);
uint64_t* init_paging_task();
uint64_t  virt_to_phys(uint64_t pml4[], uint64_t virt_addr);
uint64_t  phys_to_virt(uint64_t phys_addr);
void      write_vmem(uint64_t* pml4_addr, uint64_t virt_addr, char* data,
                     size_t len);
void      push_vmem(uint64_t* pml4_addr, uint64_t rsp, char* data, size_t len);
void map_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t phys_addr,
               uint64_t num_pages, uint64_t flags);
void alloc_pages(uint64_t pml4_addr[], uint64_t virt_addr, uint64_t num_pages,
                 uint64_t flags);


void map_sections(uint64_t pml4[]);
void map_kernel(uint64_t pml4[]);
void map_all(uint64_t pml4[]);

#endif    // __VMM_H__
