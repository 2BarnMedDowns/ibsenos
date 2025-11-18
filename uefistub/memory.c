#include <cdefs.h>
#include <efi.h>
#include <efistub.h>
#include <align.h>


extern const struct efi_system_table *ST;


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
                                EFI_LOADER_DATA, 
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
