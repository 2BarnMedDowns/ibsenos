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
typedef size_t efi_tpl;
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


/*
 * EFI boot services table
 * Contains pointers to all the boot services.
 * See section 4.4
 */

typedef enum {
    AllocateAnyPages,
    AllocateMaxAddress,
    AllocateAddress,
    MaxAllocateType
 } EFI_ALLOCATE_TYPE;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiUnacceptedMemoryType,
    EfiMaxMemoryType
 } EFI_MEMORY_TYPE;

typedef struct {
    uint32_t type;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
} EFI_MEMORY_DESCRIPTOR;

struct efi_boot_services
{
    struct efi_table_hdr hdr;

    //
    // Task Priority Services
    efi_tpl (__efiapi *raise_tpl)(efi_tpl new_tpl);
    void (__efiapi *restore_tpl)(efi_tpl old_tpl);

    //
    // Memory Services
    efi_status_t (__efiapi *allocate_pages)(EFI_ALLOCATE_TYPE type,
                                            EFI_MEMORY_TYPE memory_type,
                                            uint64_t pages,
                                            uint64_t memory);

    efi_status_t (__efiapi *free_pages)(uint64_t memory, uint64_t pages);

    efi_status_t (__efiapi *get_memory_map)(uint64_t memory_map_size,
                                            EFI_MEMORY_TYPE memory_map,
                                            uint64_t map_key,
                                            uint64_t descriptor_size,
                                            uint32_t descriptor_version);

    efi_status_t (__efiapi *allocate_pool)(EFI_MEMORY_TYPE pool_type, uint64_t size, void **buffer);
    efi_status_t (__efiapi *free_pool)(void *memory);

    //
    // Event & Timer Services
    void*        CreateEvent;    // EFI 1.0+
    void*        SetTimer;       // EFI 1.0+
    void*        WaitForEvent;   // EFI 1.0+
    void*        SignalEvent;    // EFI 1.0+
    void*        CloseEvent;     // EFI 1.0+
    void*        CheckEvent;     // EFI 1.0+

    //
    // Protocol Handler Services
    void*        InstallProtocolInterface;            // EFI 1.0+
    void*        ReinstallProtocolInterface;          // EFI 1.0+
    void*        UninstallProtocolInterface;          // EFI 1.0+
    void*        HandleProtocol;                      // EFI 1.0+
    void*        Reserved;                            // EFI 1.0+
    void*        RegisterProtocolNotify;              // EFI  1.0+
    void*        LocateHandle;                        // EFI 1.0+
    void*        LocateDevicePath;                    // EFI 1.0+
    void*        InstallConfigurationTable;           // EFI 1.0+

    //
    // Image Services
    void*        LoadImage;        // EFI 1.0+
    void*        StartImage;       // EFI 1.0+
    void*        Exit;             // EFI 1.0+
    void*        UnloadImage;      // EFI 1.0+
    void*        ExitBootServices; // EFI 1.0+

    //
    // Miscellaneous Services
    void*        GetNextMonotonicCount; // EFI 1.0+
    void*        Stall;                 // EFI 1.0+
    void*        SetWatchdogTimer;      // EFI 1.0+

    //
    // DriverSupport Services
    void*        ConnectController;     // EFI 1.1
    void*        DisconnectController;  // EFI 1.1+

    //
    // Open and Close Protocol Services
    void*        OpenProtocol;           // EFI 1.1+
    void*        CloseProtocol;          // EFI 1.1+
    void*        OpenProtocolInformation;// EFI 1.1+

    //
    // Library Services
    void*        ProtocolsPerHandle;     // EFI 1.1+
    void*        LocateHandleBuffer;     // EFI 1.1+
    void*        LocateProtocol;         // EFI 1.1+
    void*        InstallMultipleProtocolInterfaces;    // EFI 1.1+
    void*        UninstallMultipleProtocolInterfaces;   // EFI 1.1+*

    //
    // 32-bit CRC Services
    void*        CalculateCrc32;     // EFI 1.1+

    //
    // Miscellaneous Services
    void*        CopyMem;        // EFI 1.1+
    void*        SetMem;         // EFI 1.1+
    void*        CreateEventEx;  // UEFI 2.0+
};



#endif
