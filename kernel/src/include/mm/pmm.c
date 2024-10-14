#include "pmm.h"

#define PAGE_SIZE 4096

// First node
struct fl_entry* head = NULL;
// Last usable node
struct fl_entry* last = NULL;

void pmm_init() {
    // Get the first usable entry as the head of the node
    for (size_t i = 0; i < memmap->entry_count; i++) {
        if (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE) {
            struct fl_entry* first_usable =
                (struct fl_entry*)((uintptr_t)memmap->entries[i]->base
                                   + hhdm_offset);
            first_usable->next = NULL;
            head               = first_usable;
            last               = first_usable;
            break;    // Exit after the first usable entry
        }
    }

    // Add all other usable entries to the list
    for (size_t i = 0; i < memmap->entry_count; i++) {
        if (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE
            && (uintptr_t)(memmap->entries[i]->base + hhdm_offset)
                   != (uintptr_t)(head)) {

            struct fl_entry* new_entry =
                (struct fl_entry*)((uintptr_t)memmap->entries[i]->base
                                   + hhdm_offset);
            new_entry->next = NULL;
            last->next      = new_entry;
            last            = new_entry;
        }
    }

    // Terminate the free list after the last usable entry
    if (last) {
        last->next = NULL;
    }

    size_t usable_entries = (size_t)({
        size_t count = 0;
        for (size_t i = 0; i < memmap->entry_count; i++)
            count += (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE) ? 1 : 0;
        count;
    });

    printf("Initialized PMM: \n\tUsable Enties: %d\n\tHead Address: "
           "%.16llX\n\tLast Address: %.16llX\n",
           usable_entries, (uint64_t)(uintptr_t)head),
        (uint64_t)(uintptr_t)last;
}

void* pmm_req_pages(size_t num_pages) {
    if (num_pages == 0) return NULL;              // 0 pages is invalid

    size_t total_size = num_pages * PAGE_SIZE;    // Total size to allocate
    struct fl_entry* current = head;              // Start from the head
    struct fl_entry* prev    = NULL;              // Track previous entry

    while (current) {
        if (PAGE_SIZE >= total_size) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            current->next = NULL;
            return (void*)current;
        }

        prev    = current;
        current = current->next;
    }

    printf("Out of memory! Unable to allocate %zu pages!\n", num_pages);
    return NULL;
}

void pmm_free_pages(void* ptr, size_t num_pages) {
    if (ptr == NULL || num_pages == 0 || num_pages * PAGE_SIZE != PAGE_SIZE)
        return;

    struct fl_entry* free_block = (struct fl_entry*)ptr;
    free_block->next            = NULL;

    if (!head) {
        head = free_block;
        last = free_block;
    } else {
        struct fl_entry* current = head;
        struct fl_entry* prev    = NULL;

        while (current && (uintptr_t)current < (uintptr_t)free_block) {
            prev    = current;
            current = current->next;
        }

        if (prev) {
            prev->next = free_block;
        } else {
            head = free_block;
        }
        free_block->next = current;

        if (!current) {
            last = free_block;    // If it's the last block, update 'last'
        }
    }
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
    size_t num_pages[ALLOCATIONS];
    size_t allocation_count = 0;

    for (size_t i = 0; i < ALLOCATIONS; i++) {
        size_t pages_to_allocate =
            (i % MAX_PAGES) + 1;    // Allocate pages in a round-robin fashion
        void* memory = pmm_req_pages(pages_to_allocate);

        if (memory) {
            allocations[allocation_count] = memory;
            num_pages[allocation_count]   = pages_to_allocate;
            allocation_count++;
            printf("Allocated %zu pages at %p\n", pages_to_allocate, memory);
        } else {
            printf("Failed to allocate %zu pages\n", pages_to_allocate);
        }

        if (allocation_count > 0
            && (i % 3 == 0)) {    // Free every third allocation
            size_t index = (i % allocation_count);
            pmm_free_pages(allocations[index], num_pages[index]);
            printf("Freed %zu pages at %p\n", num_pages[index],
                   allocations[index]);
            allocations[index] =
                allocations[allocation_count
                            - 1];    // Move last to freed index
            num_pages[index] = num_pages[allocation_count - 1];
            allocation_count--;
        }
    }

    // Free any remaining allocations at the end
    for (size_t i = 0; i < allocation_count; i++) {
        pmm_free_pages(allocations[i], num_pages[i]);
        printf("Freed remaining %zu pages at %p\n", num_pages[i],
               allocations[i]);
    }
}
