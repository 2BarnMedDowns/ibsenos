#include "systab.h"
#include "memory.h"
#include <cdefs.h>
#include <efi.h>
#include <align.h>


efi_status_t efi_load_memory_map(void **map_ptr, struct efi_memory_map_data *map_data)
{
    efi_status_t status;
    void *ptr = NULL;
    struct efi_memory_map_data tmp = {0};

    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    if (map_ptr != NULL) {
        *map_ptr = NULL;
    }

    // First get the map size
    status = bs->get_memory_map(&tmp.map_size, NULL, &tmp.map_key,
                                &tmp.desc_size, &tmp.desc_version);
    if (status != EFI_BUFFER_TOO_SMALL) {
        // wait, what...
        return EFI_LOAD_ERROR;
    }

    if (map_ptr != NULL) {
        // Allocate memory for memory map
        // We add some additional descriptors since, according to the spec:
        // [...] the actual size of the buffer allocated for the next call to 
        // GetMemoryMap() should be bigger then[sic] the value returned in
        // MemoryMapSize, since the new buffer may potentially increase
        // memory map size.
        uint64_t size = tmp.map_size + tmp.desc_size * 32;
        ptr = efi_allocate_buffer(size, EFI_LOADER_DATA);
        if (ptr == NULL) {
            return EFI_OUT_OF_RESOURCES;
        }

        status = bs->get_memory_map(&tmp.map_size, ptr, &tmp.map_key,
                                    &tmp.desc_size, &tmp.desc_version);
        if (status != EFI_SUCCESS) {
            efi_free_buffer(ptr);
            return status;
        }

        *map_ptr = ptr;
    }

    *map_data = tmp;

    return status;
}


void * efi_allocate_buffer(size_t size, enum efi_memory_type type)
{
    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    efi_status_t status;
    void *ptr = NULL;

    if (type >= EFI_MAX_MEMORY_TYPE || type == EFI_PERSISTENT_MEMORY) {
        return NULL;
    }

    status = bs->allocate_pool(type, size, &ptr);
    if (status != EFI_SUCCESS) {
        return NULL;
    }

    return ptr;
}


void efi_free_buffer(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    bs->free_pool(ptr);
}


/*
 * 7.2.1 EFI boot services allocate pages
 */
efi_status_t efi_allocate_memory_pages(uint64_t size, uint64_t *phys_addr, uint64_t below_addr)
{
    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    efi_status_t status;
    
    // OS loaders and UEFI apps should use EFI_LOADER_DATA memory type
    // according to the specification
    enum efi_memory_type type = EFI_LOADER_DATA;

    // Since we don't know which memory addresses the UEFI firmware
    // uses for its own internal stuff, it's better to allocate
    // as large a continuous range as possible instead of trying to
    // guess fixed ranges.
    //
    // According to the spec, EFI_ALLOCATE_MAX_ADDRESS allocates 
    // any available range of pages whose uppermost address is less than 
    // or equal to the address pointed to on input. As such, set alloc_addr 
    // to be the highest possible addressable byte.
    uint64_t alloc_addr = ALIGN_DOWN(below_addr, EFI_PAGE_SIZE) - 1;

    status = bs->allocate_pages(EFI_ALLOCATE_MAX_ADDRESS,
                                type, 
                                ALIGN_DIV(size, EFI_PAGE_SIZE), // number of pages we need for size
                                &alloc_addr);
    if (status != EFI_SUCCESS) {
        return status;
    }

    *phys_addr = alloc_addr;
    return EFI_SUCCESS;
}


void efi_free_memory_pages(uint64_t size, uint64_t phys_addr)
{
    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    if (size == 0) {
        return;
    }

    uint64_t num_pages = ALIGN(size, EFI_PAGE_SIZE) >> EFI_PAGE_SHIFT;
    bs->free_pages(phys_addr, num_pages);
}
