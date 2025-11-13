#ifndef __UEFI_SYSTEM_TABLE_H__
#define __UEFI_SYSTEM_TABLE_H__

#include "uefi.h"
#include "console.h"


/*
 * EFI system table.
 * UEFI 2.11 spec, 4.
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html
 */
typedef struct {
    efi_tblhdr_t header;
    wchar_t *firmware_vendor;
    uint32_t firmware_revision;
    void *conin_handle;
    void *conin;
    void *conout_handle;
    struct efi_simple_text_output_protocol *conout;
    void *stderr_handle;
    void *stderr;
    void *runtime_services;
    void *boot_services;
    unsigned int num_cfgtbl_entries;
    void *cfgtbl;
} efi_systbl_t;
#endif
