#ifndef __UEFI_SYSTEM_TABLE_H__
#define __UEFI_SYSTEM_TABLE_H__

#include <stddef.h>
#include <stdint.h>
#include "uefi.h"
#include "console.h"


/*
 * EFI system table.
 * UEFI 2.11 spec, 4.
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html
 */
struct efi_systbl
{
    efi_tblhdr_t header;
    wchar_t *firmware_vendor;
    uint32_t firmware_revision;
    void *consolein_handle;
    void *consolein;
    void *consoleout_handle;
    struct efi_simple_text_output_protocol *consoleout;
    void *consoledbg_handle;    // StdErr is not really used
    void *consoledbg;           // StdErr is not really used
    void *rtservices;
    void *boot_services;
    unsigned int num_cfgtbl_entries;
    void *cfgtbl;
};

#endif
