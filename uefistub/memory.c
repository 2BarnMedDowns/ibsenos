#include <cdefs.h>
#include <efi.h>
#include <efistub.h>
#include <align.h>
#include <string.h>


extern const struct efi_system_table *ST;


efi_status_t efi_get_memory_map(struct efi_memory_map **map)
{
    struct efi_memory_map *m = NULL;
    uint32_t desc_version;
    uint64_t desc_size;
    uint64_t map_size;
    uint64_t map_key;

    *map = NULL;

    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    // First get some map metadata
    map_size = 0;
    efi_status_t status = bs->get_memory_map(&map_size, NULL, &map_key,
                                             &desc_size, &desc_version);
    if (status != EFI_BUFFER_TOO_SMALL) {
        return EFI_LOAD_ERROR;
    }

    // FIXME: if we want to use the map after BootServicesExit, 
    // we should probably use EFI_ACPI_RECLAIM_MEMORY 
    // (and also install a configuration table)
    enum efi_memory_type type = EFI_LOADER_DATA;

    // Allocate memory for memory map
    uint64_t size = map_size + desc_size * 32; // Need extra padding here otherwise it randomly fails
    status = bs->allocate_pool(EFI_LOADER_DATA, 
                               sizeof(*m) + size,
                               (void**) &m);
    if (status != EFI_SUCCESS) {
        return status;
    }

    m->buffer_size = m->map_size = size;
    status = bs->get_memory_map(&m->map_size, m->descriptors, &m->key,
                                &m->descriptor_size, &m->descriptor_version);
    if (status != EFI_SUCCESS) {
        bs->free_pool((void*) m);
        return status;
    }

    m->num_descriptors = m->map_size / m->descriptor_size;

    *map = m;
    return EFI_SUCCESS;
}


/*
 * 7.2.1 EFI boot services allocate pages
 */
efi_status_t efi_allocate_memory(uint64_t size, uint64_t *phys_addr, uint64_t below_addr)
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


void efi_free_memory(uint64_t size, uint64_t phys_addr)
{
    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    if (size == 0) {
        return;
    }

    uint64_t num_pages = ALIGN(size, EFI_PAGE_SIZE) >> EFI_PAGE_SHIFT;
    bs->free_pages(phys_addr, num_pages);
}
