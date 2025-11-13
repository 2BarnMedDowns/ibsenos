#ifndef __UEFI_H__
#define __UEFI_H__

/*
 * Latest UEFI specification: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include <stddef.h>
#include <stdint.h>


/*
 * EFI/UEFI uses MS ABI calling convention instead of cdecl.
 */
#define EFIAPI __attribute__((ms_abi))


/*
 * Some status/error codes.
 */
enum {
    EFI_SUCCESS = 0,
    EFI_ERROR = 1
};

typedef unsigned int efi_status_t;


/*
 * UUID/GUID type - RFC 4122
 */
typedef unsigned char uuid_t[16];


/*
 * Physical address type.
 */
typedef uint64_t phaddr_t;


/*
 * Virtual address type;
 */
typedef uint64_t vaddr_t;


/*
 * Common EFI table header (4.2.1 in spec)
 * Precedes all the standard EFI table types.
 */
typedef struct {
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t reserved;
} efi_tblhdr_t;


//struct time
//{
//    uint16_t year;
//    uint8_t month;
//    uint8_t day;
//    uint8_t hour;
//    uint8_t minute;
//    uint8_t second;
//    uint8_t pad1;
//    uint32_t nanosecond;
//    int16_t timezone;
//    uint8_t daylight;
//    uint8_t pad2;
//};


#endif
