#ifndef __EFI_H__
#define __EFI_H__

/*
 * Latest UEFI specification: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include "compilerattrs.h"
#include <stddef.h>
#include <stdint.h>


/* Some EFI types */
typedef unsigned int efi_status_t;
typedef uint16_t efi_char16_t;

//typedef uint8_t efi_bool_t;
//typedef uint64_t efi_phys_addr_t;
//typedef void * efi_handle_t;


/*
 * EFI/UEFI uses MS ABI calling convention instead of cdecl.
 */
#define __efiapi __attribute__((ms_abi))


/*
 * Some EFI status/error codes.
 */
#define EFI_SUCCESS		        0
#define EFI_LOAD_ERROR		    ( 1 | (1UL << 63))
#define EFI_INVALID_PARAMETER   ( 2 | (1UL << 63))
#define EFI_UNSUPPORTED         ( 3 | (1UL << 63))
#define EFI_BAD_BUFFER_SIZE     ( 4 | (1UL << 63))
#define EFI_BUFFER_TOO_SMALL    ( 5 | (1UL << 63))
#define EFI_NOT_READY           ( 6 | (1UL << 63))
#define EFI_DEVICE_ERROR        ( 7 | (1UL << 63))
#define EFI_WRITE_PROTECTED     ( 8 | (1UL << 63))
#define EFI_OUT_OF_RESOURCES    ( 9 | (1UL << 63))
#define EFI_NOT_FOUND           (14 | (1UL << 63))
#define EFI_ACCESS_DENIED       (15 | (1UL << 63))
#define EFI_TIMEOUT             (18 | (1UL << 63))
#define EFI_ABORTED             (21 | (1UL << 63))
#define EFI_SECURITY_VIOLATION  (26 | (1UL << 63))


typedef struct __aligned(__alignof__(uint32_t)) {
    uint8_t bytes[16];
} efi_guid_t;


#define EFI_GUID(a, b, c, d...) ((efi_guid_t){ { \
        (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
        (b) & 0xff, ((b) >> 8) & 0xff, \
        (c) & 0xff, ((c) >> 8) & 0xff, d } })


/*
 * Common EFI table header (4.2.1 in spec)
 * Precedes all the standard EFI table types.
 *
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#efi-table-header
 */
typedef struct efi_table_header
{
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t reserved;
} efi_table_hdr_t;


/*
 * UEFI 2.11 spec, 12.4.1
 * https://uefi.org/specs/UEFI/2.11/12_Protocols_Console_Support.html#efi-simple-text-output-protocol
 */
typedef struct efi_simple_text_output_protocol efi_simple_text_output_protocol_t;

struct efi_simple_text_output_protocol
{
    void *reset;
    efi_status_t (__efiapi *output_string)(efi_simple_text_output_protocol_t*,
                                           efi_char16_t*);
    void *test_string;
};


/*
 * UEFI 2.11 spec, 4.3
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#efi-system-table-1
 */
struct efi_system_table
{
    efi_table_hdr_t hdr;
    uint64_t fw_vendor; // physical address of CHAR16 vendor string
    uint32_t fw_revision;
    uint32_t pad1;      // manual alignment
    uint64_t con_in_handle;
    uint64_t con_in;
    uint64_t con_out_handle;
    uint64_t con_out;
    uint64_t stderr_handle;
    uint64_t stder;
    uint64_t rt;
    uint64_t bt;
    uint32_t num_tables;
    uint32_t pad2;      // manual alignment
    uint64_t tables;
};

#define EFI_SYSTEM_TABLE_SIGNATURE ((uint64_t) 0x5453595320494249ULL)


#endif
