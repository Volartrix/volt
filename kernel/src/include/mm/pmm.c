#include "pmm.h"

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y)     DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y)   (x / y) * y

#define PAGE_SIZE 4096

uint8_t* bitmap;
uint64_t bitmap_pages;
uint64_t bitmap_size;

void bitmap_set(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] |= 1 << (bit % 8);
}

void bitmap_clear(uint8_t* bitmap, uint64_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

uint8_t bitmap_get(uint8_t* bitmap, uint64_t bit) {
    return bitmap[bit / 8] & (1 << (bit % 8));
}

void pmm_init() {
    uint64_t top_address;
    uint64_t higher_address = 0;

    for (uint64_t entryCount = 0; entryCount < memmap->entry_count;
         entryCount++) {
        struct limine_memmap_entry* entry = memmap->entries[entryCount];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            top_address = entry->base + entry->length;
            if (top_address > higher_address) higher_address = top_address;
        }
    }
    bitmap_pages = higher_address / PAGE_SIZE;
    bitmap_size  = ALIGN_UP(bitmap_pages / 8, PAGE_SIZE);

    for (uint64_t entryCount = 0; entryCount < memmap->entry_count;
         entryCount++) {
        struct limine_memmap_entry* entry = memmap->entries[entryCount];

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            if (entry->length >= bitmap_size) {
                bitmap = (uint8_t*)(entry->base + hhdm_offset);
                memset(bitmap, 0xFF, bitmap_size);
                entry->base   += bitmap_size;
                entry->length -= bitmap_size;
                break;
            }
        }
    }

    for (uint64_t entryCount = 0; entryCount < memmap->entry_count;
         entryCount++) {
        struct limine_memmap_entry* entry = memmap->entries[entryCount];

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t i = 0; i < entry->length; i += PAGE_SIZE) {
                bitmap_clear(bitmap, (entry->base + i) / PAGE_SIZE);
            }
        }
    }

    printf("Successfully initialized PMM: \n\tTop Address: 0x%.16llX "
           "\n\tHigher Address: 0x%.16llX \n\tBitmap Address: %p \n\tBitmap "
           "Pages: 0x%.16llX \n\tBitmap Size: %.16llX\n",
           top_address, higher_address, bitmap, bitmap_pages, bitmap_size);
}

void* pmm_req_pages(size_t num_pages) {
    uint64_t last_allocated_index = 0;

    while (1) {
        if (!bitmap_get(bitmap, last_allocated_index)) {
            size_t consecutive_free_pages = 1;

            for (size_t i = 1; i < num_pages; i++) {
                if (!bitmap_get(bitmap, last_allocated_index + 1)) {
                    ++consecutive_free_pages;
                } else {
                    consecutive_free_pages = 0;
                    break;
                }
            }

            if (consecutive_free_pages == num_pages) {
                for (size_t i = 0; i < num_pages; i++) {
                    bitmap_set(bitmap, last_allocated_index + i);
                }

                return (void*)(last_allocated_index * PAGE_SIZE);
            }
        }

        ++last_allocated_index;

        if (last_allocated_index >= bitmap_pages) {
            return NULL;
        }
    }
}

void pmm_free_pages(void* ptr, size_t num_pages) {
    if (ptr == NULL) {
        printf("ptr is null you dumbass");
        asm("hlt");
    }
    if ((uint64_t)ptr % PAGE_SIZE != 0) {
        printf("ptr isn't devidable through the page size you dumbass");
        asm("hlt");
    }
    if (num_pages <= 0) {
        printf("The number of pages is 0 you dumbass");
        asm("hlt");
    }

    uint64_t start_bit_idx = ((uint64_t)ptr / PAGE_SIZE);

    for (size_t i = start_bit_idx; i < num_pages; ++i) {
        bitmap_clear(bitmap, i);
    }
}

uint64_t pmm_get_free() {
    uint64_t freeMemory = 0;
    for (uint64_t i = 0; i < bitmap_pages; i++) {
        if (!bitmap_get(bitmap, i)) {
            freeMemory += PAGE_SIZE;
        }
    }

    return freeMemory;
}
