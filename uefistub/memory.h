#ifndef __IBSENOS_UEFISTUB_MEMORY_H__
#define __IBSENOS_UEFISTUB_MEMORY_H__

#include <efi.h>
#include <inttypes.h>


struct efi_memory_map_data
{
    uint64_t phys_map;      // physical address
    uint64_t map_key;       // memory map key, must be provided to ExitBootServices
    uint64_t map_size;      // total size of the memory map
    uint32_t desc_version;  // descriptor version (should be the same as EFI_MEMORY_DESCRIPTOR_VERSION otherwise we're in trouble)
    uint64_t desc_size;     // size of each memory map descriptor entry
};


#define efi_memory_desc_ptr(map_ptr, desc_size, idx) \
    ((struct efi_memory_desc*) (((uint8_t*) (map_ptr)) + desc_size * idx))



efi_status_t efi_load_memory_map(void **map_ptr,
                                 struct efi_memory_map_data *map_data);


/*
 * Allocate a range of memory (pages) below the address given by below_addr.
 */
efi_status_t efi_allocate_memory_pages(uint64_t size, 
                                       uint64_t *phys_addr,
                                       uint64_t below_addr);


/*
 * Free memory pages.
 */
void efi_free_memory_pages(uint64_t size, uint64_t phys_addr);



/*
 * Allocate a memory buffer for use in the UEFI application.
 *
 * If you're unsure what memory type to use, you probably want to
 * use EFI_LOADER_DATA.
 */
void * efi_allocate_buffer(size_t size, enum efi_memory_type type);


/*
 * Free memory buffer.
 */
void efi_free_buffer(void *ptr);


#endif
