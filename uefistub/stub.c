#include <efi.h>
#include "memory.h"
#include "console.h"
#include <inttypes.h>
#include <string.h>


const struct efi_system_table *ST = NULL;


#ifdef DEBUG
static void print_uefi_info(void)
{
    uint32_t uefirev = ST->hdr.revision;
    uint32_t uefi_major = (uefirev >> 16);
    uint32_t uefi_minor = (uefirev & 0xffff) / 10;
    uint32_t uefi_tertiary = uefirev % 10;

    uint32_t fwrev_major = (ST->fw_revision >> 16);
    uint32_t fwrev_minor = (ST->fw_revision & 0xffff);

    efi_puts("UEFI revision: ");
    
    efi_console_color(EFI_CONSOLE_BRIGHTCYAN);
    efi_putd(uefi_major);
    efi_puts(".");
    efi_putd(uefi_minor);
    efi_puts(".");
    efi_putd(uefi_tertiary);
    efi_console_restore();
    efi_puts("\n");

    efi_puts("Firmware vendor: ");
    efi_console_color(EFI_CONSOLE_BRIGHTMAGENTA);
    efi_char16_puts((const uint16_t*) ST->fw_vendor);
    efi_console_restore();
    efi_puts("\n");

    efi_puts("Firmware revision: ");
    efi_console_color(EFI_CONSOLE_YELLOW);
    efi_putd(fwrev_major);
    efi_puts(".");
    efi_putd(fwrev_minor);
    efi_console_restore();
    efi_puts("\n");

    efi_puts("\n");
}
#endif


/*
 * EFI image entry point
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#uefi-image-entry-point
 */
efi_status_t __efiapi uefistub_pe_entry(void *, struct efi_system_table *systab)
{
    efi_status_t status;
    ST = NULL;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        return EFI_INVALID_PARAMETER;
    }

    ST = systab;

    efi_console_reset();
    efi_console_clear_screen();

#ifdef DEBUG
    print_uefi_info();
#endif

    uint64_t phys_addrs[3] = {0};  // phys_addrs[0] to see that there is no memory map table when we haven't allocated anything
    uint64_t num_pages_per_chunk = 1;

    // Allocate some memory
    for (size_t i = 0; i < array_size(phys_addrs); ++i) {
        efi_puts("Allocating memory...");

        status = efi_allocate_memory_pages(num_pages_per_chunk * EFI_PAGE_SIZE, &phys_addrs[i], 1 << 30);
        
        efi_puts(status == EFI_SUCCESS ? "SUCCESS" : "FAILED");
        efi_puts("\n");
        
        if (status == EFI_SUCCESS) {
            efi_puts("Address: ");
            efi_puth(phys_addrs[i]);
        } else {
            efi_puts("Error code: ");
            efi_puth(status);
        }
        efi_puts("\n");
        efi_puts("\n");
    }

    // Get the memory map
    struct efi_memory_map_data map_info = {0};
    void *map = NULL;
    status = efi_load_memory_map(&map, &map_info);
    if (status != EFI_SUCCESS) {
        efi_puts("Failed to get memory map or there is no allocated memory\n");
        goto leave;
    }

    uint64_t num_entries = map_info.map_size / map_info.desc_size;
    efi_puts("Number of memory map entries: ");
    efi_putd(num_entries);
    efi_puts("\n\n");

    for (uint64_t idx = 0; idx < num_entries; ++idx) {
        struct efi_memory_desc *md = efi_memory_desc_ptr(map, map_info.desc_size, idx);

        if (md->type == EFI_LOADER_DATA) {
            efi_puts("Entry ");
            efi_putd(idx);
            efi_puts(" is type EfiLoaderData\n");
            efi_puts("Physical start address: ");
            efi_puth(md->phys_start);
            efi_puts("\n");
            efi_puts("Number of pages: ");
            efi_putd(md->num_pages);
            efi_puts("\n");
            efi_puts("Memory attributes: \n");
            efi_put0h(md->attribute);
            efi_puts("\n\n");
        }
    }

leave:
    // We must free all memory allocated here, according to the 7.2. part of the spec
    // if we want to move some memory, we need to install stuff into the configuration table
    efi_free_buffer(map);

    for (size_t i = 0; i < array_size(phys_addrs); ++i) {
        efi_free_memory_pages(num_pages_per_chunk * EFI_PAGE_SIZE, phys_addrs[i]);
    }
    
    efi_puts("Bye\n");

    // call ExitBootServices in future
    for (;;) {}
}
