#ifndef __PMM_H__
#define __PMM_H__

#include <limine.h>
#include <mm/vmm.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// For the stress test
#define MAX_PAGES   16
#define ALLOCATIONS 100

extern struct limine_memmap_response* memmap;
extern uint64_t                       hhdm_offset;
extern uint64_t*                      pml4_global;

struct fl_entry {
    struct fl_entry* next;
    size_t size;
};

void     pmm_init();
void*    pmm_req_pages(size_t num_pages);
void     pmm_free_pages(void* ptr, size_t num_pages);
uint64_t pmm_get_free();
void     pmm_stress_test();


#endif    // __PMM_H__
