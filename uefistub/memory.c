#include "efistub.h"
#include <cdefs.h>
#include <efi.h>
#include <align.h>



//efi_status_t efi_get_memory_map(struct efi_memory_map **map)
//{
//    struct efi_memory_map *m = NULL;
//    uint32_t desc_version;
//    uint64_t desc_size = 0;
//    uint64_t map_size;
//    uint64_t map_key;
//
//    *map = NULL;
//
//    const struct efi_boot_services *bs = 
//        (const struct efi_boot_services*) ST->boot_services;
//
//    // First get the map size
//    map_size = 0;
//    efi_status_t status = bs->get_memory_map(&map_size, NULL, &map_key,
//                                             &desc_size, &desc_version);
//    if (status != EFI_BUFFER_TOO_SMALL) {
//        return EFI_LOAD_ERROR;
//    }
//
//    // Do some sanity checking
//    if (desc_version != EFI_MEMORY_DESCRIPTOR_VERSION) {
//        efi_puts("version differ\n");
//        return EFI_LOAD_ERROR;
//    }
//
//    if (desc_size != sizeof(struct efi_memory_desc)) {
//        efi_puts("desc_size: ");
//        efi_putd(desc_size);
//        efi_puts(" , sizeof: ");
//        efi_putd(sizeof(struct efi_memory_desc));
//        efi_puts("\n");
//        efi_puts("desc size differs, some alignment bogus stuff\n");
//        return EFI_LOAD_ERROR;
//    }
//
//    // FIXME: if we want to use the map after BootServicesExit, 
//    // we should probably use EFI_ACPI_RECLAIM_MEMORY 
//    // (and also install a configuration table)
//    enum efi_memory_type type = EFI_LOADER_DATA;
//
//    // Allocate memory for memory map
//    //
//    // We add some additional descriptors since, according to the spec:
//    // [...] the actual size of the buffer allocated for the next call to 
//    // GetMemoryMap() should be bigger then[sic] the value returned in
//    // MemoryMapSize, since the new buffer may potentially increase
//    // memory map size.
//    uint64_t size = map_size + desc_size * 32; 
//
//    status = bs->allocate_pool(type, 
//                               sizeof(*m) + size,
//                               (void**) &m);
//    if (status != EFI_SUCCESS) {
//        return status;
//    }
//
//    m->buffer_size = m->map_size = size;
//    status = bs->get_memory_map(&m->map_size, m->map, &m->map_key,
//                                &m->desc_size, &m->desc_version);
//    if (status != EFI_SUCCESS) {
//        bs->free_pool((void*) m);
//        return status;
//    }
//
//    m->map_entries = m->map_size / m->desc_size;
//
//    *map = m;
//    return EFI_SUCCESS;
//}


void * efi_malloc(size_t size, enum efi_memory_type type)
{
    const struct efi_boot_services *bs = 
        (const struct efi_boot_services*) ST->boot_services;

    efi_status_t status;
    void *ptr = NULL;

    status = bs->allocate_pool(type, size, &ptr);
    if (status != EFI_SUCCESS) {
        return NULL;
    }

    return ptr;
}


void efi_free(void *ptr)
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
