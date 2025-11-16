#ifndef __IBSENOS_UEFISTUB_EFI_H__
#define __IBSENOS_UEFISTUB_EFI_H__

/*
 * Implemented following the UEFI 2.11 specification
 * See: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include <compiler.h>
#include <cdefs.h>
#include <inttypes.h>


/* Some EFI types */
typedef uint64_t efi_status_t;
typedef void * efi_handle_t;


/*
 * EFI/UEFI uses MS ABI calling convention instead of cdecl.
 */
#define __efiapi __attribute__((ms_abi))


/*
 * Some EFI status/error codes.
 */
#define EFI_SUCCESS		        0
#define EFI_LOAD_ERROR		    ( 1 | (1ULL << 63))
#define EFI_INVALID_PARAMETER   ( 2 | (1ULL << 63))
#define EFI_UNSUPPORTED         ( 3 | (1ULL << 63))
#define EFI_BAD_BUFFER_SIZE     ( 4 | (1ULL << 63))
#define EFI_BUFFER_TOO_SMALL    ( 5 | (1ULL << 63))
#define EFI_NOT_READY           ( 6 | (1ULL << 63))
#define EFI_DEVICE_ERROR        ( 7 | (1ULL << 63))
#define EFI_WRITE_PROTECTED     ( 8 | (1ULL << 63))
#define EFI_OUT_OF_RESOURCES    ( 9 | (1ULL << 63))
#define EFI_NOT_FOUND           (14 | (1ULL << 63))
#define EFI_ACCESS_DENIED       (15 | (1ULL << 63))
#define EFI_TIMEOUT             (18 | (1ULL << 63))
#define EFI_ABORTED             (21 | (1ULL << 63))
#define EFI_SECURITY_VIOLATION  (26 | (1ULL << 63))


typedef struct __aligned(__alignof__(uint32_t)) {
    uint8_t bytes[16];
} efi_guid_t;


#define EFI_GUID(a, b, c, d...) ((efi_guid_t){ { \
        (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff, \
        (b) & 0xff, ((b) >> 8) & 0xff, \
        (c) & 0xff, ((c) >> 8) & 0xff, d } })


/*
 * Common EFI table header.
 * Precedes all the standard EFI table types.
 * See section 4.2.1
 */
struct efi_table_hdr
{
    uint64_t signature;
    uint32_t revision;      // revision is in (major).(minor upper).(minor lower) format
    uint32_t header_size;
    uint32_t crc32;
    uint32_t reserved;
};


/*
 * EFI system table.
 * Contains pointers to the runtime and boot services
 * tables, as well as simple console protocols.
 * See section 4.3.1
 */
struct efi_system_table
{
    struct efi_table_hdr hdr;
    uint64_t fw_vendor;     // physical address of CHAR16 vendor string
    uint32_t fw_revision;
    uint32_t pad1;          // manual alignment to 64-bit
    uint64_t console_in_handle;
    uint64_t console_in;    // physical address to console input protocol
    uint64_t console_out_handle;
    uint64_t console_out;   // physical address to console output protocol
    uint64_t stderr_handle;
    uint64_t stder;         // stderr is not really used
    uint64_t rt;            // runtime services
    uint64_t bt;            // boot services
    uint32_t num_tables;    // number of configuration tables
    uint32_t pad2;          // manual alignment to 64-bit
    uint64_t tables;        // physical address to configuration tables
};

#define EFI_SYSTEM_TABLE_SIGNATURE ((uint64_t) 0x5453595320494249ULL)

extern const struct efi_system_table *ST;


/*
 * EFI boot services table
 * Contains pointers to all the boot services.
 * See section 4.4
 */
struct efi_boot_services
{
    struct efi_table_hdr hdr;
};



#endif
