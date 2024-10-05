#ifndef __PMM_H__
#define __PMM_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <limine.h>
#include <string.h>

extern struct limine_memmap_response *memmap;
extern uint64_t hhdm_offset;

void pmm_init();
void *pmm_req_pages(size_t num_pages);
void pmm_free_pages(void *ptr, size_t num_pages);
uint64_t pmm_get_free();


#endif // __PMM_H__