#include "pmm.h"

#define PAGE_SIZE 4096

// First node
struct fl_entry* head = NULL;

void pmm_init() {
    for (size_t i = 0; i < memmap->entry_count; i++) {
        if (memmap->entries[i]->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        if (head == NULL) {
            head = (struct fl_entry*)(memmap->entries[i]->base + hhdm_offset);
            head->next = NULL;
            head->size = memmap->entries[i]->length; // length is always page aligned (limine spec.)
            continue;
        }

        struct fl_entry* entry = (struct fl_entry*)(memmap->entries[i]->base + hhdm_offset);
        entry->next = head;
        entry->size = memmap->entries[i]->length;
        head = entry;
    }

    size_t usable_entries = (size_t)({
        size_t count = 0;
        for (size_t i = 0; i < memmap->entry_count; i++)
            count += (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE) ? 1 : 0;
        count;
    });

    printf("Initialized PMM: \n\tUsable Enties: %d\n\tHead Address: "
           "%.16llX\n",
           usable_entries, (uint64_t)(uintptr_t)(head - hhdm_offset));
}

void* pmm_req_page() {
    if (head == NULL) {
        printf("There is no free memory!");
        return NULL; // No memory is free.
    }

    if (head->size == 4096) {
        void* pointer = (void*)head;
        head = head->next;
        if (pointer == NULL)
            printf("Ptr is null :P 1.\n");
        return pointer - hhdm_offset;
    }

    // Size is greater than 4096.
    struct fl_entry* new_head = (struct fl_entry*) ((uintptr_t)head + 4096); // 
    new_head->next = head->next;
    new_head->size = head->size - 4096;
    void* pointer = (void*) head;
    head = new_head;
    if (pointer == NULL) {
        printf("Ptr is null :P 2.\n");
    }
    printf("Allocation ouccoured! Memory allocated: %d, Free Memory: %d, Address: %.16llX\n", head->size, pmm_get_free(), (uint64_t)(pointer - hhdm_offset));
    return pointer - hhdm_offset;
}

void pmm_free_page(void* ptr) {
    if (ptr == NULL)
        return;

    ptr += hhdm_offset;

    struct fl_entry* entry = (struct fl_entry*)ptr;
    entry->size = 4096;
    entry->next = head;
    head = entry;
}

uint64_t pmm_get_free() {
    uint64_t         total_free = 0;
    struct fl_entry* current    = head;

    while (current) {
        total_free += PAGE_SIZE;
        current     = current->next;
    }

    return total_free;
}

void pmm_stress_test() {
    void*  allocations[ALLOCATIONS];
    size_t allocation_count = 0;

    for (size_t i = 0; i < ALLOCATIONS; i++) {
        void* memory = pmm_req_page();    // Allocate a single page

        if (memory) {
            allocations[allocation_count] = memory;
            allocation_count++;
            printf("Allocated 1 page at %p\n", memory);
        } else {
            printf("Failed to allocate 1 page\n");
        }

        if (allocation_count > 0 && (i % 3 == 0)) {    // Free every third allocation
            size_t index = (i % allocation_count);
            pmm_free_page(allocations[index]);         // Free a single page
            printf("Freed 1 page at %p\n", allocations[index]);
            allocations[index] = allocations[allocation_count - 1];    // Move last to freed index
            allocation_count--;
        }
    }

    // Free any remaining allocations at the end
    for (size_t i = 0; i < allocation_count; i++) {
        pmm_free_page(allocations[i]);    // Free a single page
        printf("Freed remaining 1 page at %p\n", allocations[i]);
    }
}

void* pmm_req_pages(size_t num_pages) {
    void* page_array = pmm_req_page(); // Allocate a single page for the array of pointers
    if (!page_array) {
        return NULL; // Return NULL if allocation fails
    }

    // Store the individual pages right after the array
    void** pages = (void**)page_array;

    for (size_t i = 0; i < num_pages; i++) {
        pages[i] = pmm_req_page(); // Request each individual page
        if (!pages[i]) {
            // If allocation fails, free already allocated pages
            for (size_t j = 0; j < i; j++) {
                pmm_free_page(pages[j]);
            }
            pmm_free_page(page_array); // Free the array holding the pointers
            return NULL; // Return NULL if any page request fails
        }
    }

    return page_array; // Return the pointer to the array of pages
}


void pmm_free_pages(void* pages, size_t num_pages) {
    // Assuming pages is the pointer to the array of pointers
    void** page_ptrs = (void**)pages; // Cast to the correct pointer type

    for (size_t i = 0; i < num_pages; i++) {
        pmm_free_page(page_ptrs[i]); // Free each allocated page
    }
    pmm_free_page(pages); // Free the page that holds the pointers
}
