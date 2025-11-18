/*
 * EFI library stub functions.
 */
#ifndef __IBSENOS_UEFISTUB_EFI_STUB_LIBRARY_H__
#define __IBSENOS_UEFISTUB_EFI_STUB_LIBRARY_H__

#include <efi.h>
#include <inttypes.h>
#include "efi_console.h"


extern const struct efi_system_table *ST;


//struct efi_memory_map_data
//{
//};


//efi_status_t efi_get_memory_map(struct efi_memory_map_data **memmap);


/*
 * Allocate a range of memory (pages) below the address given by below_addr
 */
efi_status_t efi_allocate_memory_pages(uint64_t size, 
                                       uint64_t *phys_addr,
                                       uint64_t below_addr);


/*
 * Free memory range.
 */
void efi_free_memory_pages(uint64_t size, uint64_t phys_addr);



/*
 * Allocate a memory buffer for use in the UEFI application.
 *
 * If you're unsure what memory type to use, you probably want to
 * use EFI_LOADER_DATA.
 */
void * efi_malloc(size_t size, enum efi_memory_type type);


/*
 * Free UEFI application memory.
 */
void efi_free(void *ptr);


#endif
