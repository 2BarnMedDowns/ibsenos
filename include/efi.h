/*
 * This file should only contain definitions from the UEFI 2.11 specification.
 * See: https://uefi.org/specs/UEFI/2.11/index.html
 *
 * For helper functions and types, see the uefistub implementation.
 */
#ifndef __IBSENOS_EFI_H__
#define __IBSENOS_EFI_H__

#include <compiler.h>
#include <cdefs.h>
#include <inttypes.h>


/* Some EFI types */
typedef uint64_t efi_status_t;
typedef void * efi_handle_t;
typedef void * efi_event_t;


/*
 * EFI/UEFI uses MS ABI calling convention instead of cdecl.
 */
#define __efiapi __attribute__((ms_abi))


#define __efi_nargs(...) __VA_NARGS__(__VA_ARGS__)


#if defined(__MINGW32__)
#define efiapi_call(func, ...) func(__VA_ARGS__)
#else
#error "Unsupported cdecl"
#endif


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
    uint64_t stderr;        // stderr is not really used
    uint64_t runtime_services;  // runtime services
    uint64_t boot_services; // boot services
    uint32_t num_tables;    // number of configuration tables
    uint32_t pad2;          // manual alignment to 64-bit
    uint64_t tables;        // physical address to configuration tables
};

#define EFI_SYSTEM_TABLE_SIGNATURE ((uint64_t) 0x5453595320494249ULL)


/* 
 * EFI configuration table and GUID definitions
 */
#define NULL_GUID                               EFI_GUID(0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
#define EFI_CONSOLE_OUT_DEVICE_GUID             EFI_GUID(0xd3b36f2c, 0xd551, 0x11d4, 0x9a, 0x46, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d)
#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID    EFI_GUID(0x387477c2, 0x69c7, 0x11d2, 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b)
#define EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID     EFI_GUID(0x387477c1, 0x69c7, 0x11d2, 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b)
#define EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID       EFI_GUID(0x9042a9de, 0x23dc, 0x4a38, 0x96, 0xfb, 0x7a, 0xde, 0xd0, 0x80, 0x51, 0x6a)


struct efi_config_table
{
    efi_guid_t guid;    // VendorGuid
    uint64_t table;     // VendorTable
};




/* 
 * EFI task priority levels
 */
#define EFI_TPL_APPLICATION	4
#define EFI_TPL_CALLBACK	8
#define EFI_TPL_NOTIFY		16
#define EFI_TPL_HIGH_LEVEL	31

/*
 * EFI event types
 * These types can be OR'd together as needed.
 */
#define EFI_EVT_TIMER                           0x80000000
#define EFI_EVT_RUNTIME                         0x40000000
#define EFI_EVT_NOTIFY_WAIT                     0x00000100
#define EFI_EVT_NOTIFY_SIGNAL                   0x00000200
#define EFI_EVT_SIGNAL_EXIT_BOOT_SERVICES       0x00000201
#define EFI_EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE   0x00000202


enum efi_timer_delay
{
    EFI_TIMER_CANCEL = 0,
    EFI_TIMER_PERIODIC = 1,
    EFI_TIMER_RELATIVE = 2
};


/*
 * EFI allocation types
 */
enum efi_allocate_type 
{
    EFI_ALLOCATE_ANY_PAGES = 0,
    EFI_ALLOCATE_MAX_ADDRESS = 1,
    EFI_ALLOCATE_ADDRESS = 2,
    EFI_MAX_ALLOCATE_TYPE
 };


/* 
 * EFI memory types
 * We're paranoid, so we set the values explicitly,
 * even though enums should start at 0.
 */
enum efi_memory_type 
{
    EFI_RESERVED_MEMORY_TYPE = 0,
    EFI_LOADER_CODE = 1,
    EFI_LOADER_DATA = 2,
    EFI_BOOT_SERVICES_CODE = 3,
    EFI_BOOT_SERVICES_DATA = 4,
    EFI_RUNTIME_SERVICES_CODE = 5,
    EFI_RUNTIME_SERVICES_DATA = 6,
    EFI_CONVENTIONAL_MEMORY = 7,
    EFI_UNUSABLE_MEMORY = 8,
    EFI_ACPI_RECLAIM_MEMORY = 9,
    EFI_ACPI_MEMORY_NVS = 10,
    EFI_MEMORY_MAPPED_IO = 11,
    EFI_MEMORY_MAPPED_IO_PORT_SPACE = 12,
    EFI_PAL_CODE = 13,
    EFI_PERSISTENT_MEMORY = 14,
    EFI_UNACCEPTED_MEMORY_TYPE = 15,
    EFI_MAX_MEMORY_TYPE
};


enum efi_interface_type
{
    EFI_NATIVE_INTERFACE = 0,
};


/* 
 * Memory attribute values.
 * Can be OR'd together.
 */
#define EFI_MEMORY_UC               (1ULL <<  0)    /* uncached */
#define EFI_MEMORY_WC               (1ULL <<  1)    /* write-coalescing */
#define EFI_MEMORY_WT               (1ULL <<  2)    /* write-through */
#define EFI_MEMORY_WB               (1ULL <<  3)    /* write-back */
#define EFI_MEMORY_UCE              (1ULL <<  4)    /* uncached, exported */
#define EFI_MEMORY_WP               (1ULL << 12)    /* write protect */
#define EFI_MEMORY_RP               (1ULL << 13)    /* read protect */
#define EFI_MEMORY_XP               (1ULL << 14)    /* execute protect */
#define EFI_MEMORY_NV               (1ULL << 15)    /* non-volatile */
#define EFI_MEMORY_MORE_RELIABLE    (1ULL << 16)    /* higher reliability */
#define EFI_MEMORY_RO               (1ULL << 17)    /* read-only */
#define EFI_MEMORY_SP               (1ULL << 18)    /* soft reserved */
#define EFI_MEMORY_CPU_CRYPTO       (1ULL << 19)    /* supports encryption */
#define EFI_MEMORY_HOT_PLUGGABLE    (1ULL << 20)    /* supports unplugging at runtime */
#define EFI_MEMORY_RUNTIME          (1ULL << 63)    /* memory range requires runtime mapping */


/*
 * EFI memory map descriptor.
 */
struct efi_memory_desc
{
    enum efi_memory_type type;
    uint32_t pad;
    uint64_t phys_start;
    uint64_t virt_start;
    uint64_t num_pages;
    uint64_t attribute;
};


#define EFI_MEMORY_DESCRIPTOR_VERSION 1


#define EFI_PAGE_SHIFT 12
#define EFI_PAGE_SIZE (1UL << EFI_PAGE_SHIFT)
#define EFI_PAGES_MAX (UINT64_MAX >> EFI_PAGE_SHIFT)


/*
 * Event notification callback.
 */
typedef void (__efiapi *efi_event_notify_t)(efi_event_t *event, 
                                            void *context);


enum efi_locate_search_type
{
    EFI_LOCATE_ALL_HANDLES = 0,
    EFI_LOCATE_BY_REGISTER_NOTIFY = 1,
    EFI_LOCATE_BY_PROTOCOL = 2
};


/* Attributes for BootServices.OpenProtocol */
#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL   0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL         0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL        0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER  0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER            0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE            0x00000020


/*
 * Protocol information.
 */
struct efi_open_protocol_information_entry
{
    efi_handle_t agent;
    efi_handle_t controller;
    uint32_t attributes;
    uint32_t pad0;
    uint32_t open_count;
    uint32_t pad1;
};


/*
 * EFI boot services table
 * Contains pointers to all the boot services.
 * See section 4.4
 */
struct efi_boot_services
{
    struct efi_table_hdr hdr;

    // Task Priority Services
    uint64_t (__efiapi *raise_tpl)(uint64_t new_tpl);
    void (__efiapi *restore_tpl)(uint64_t old_tpl);

    // Memory Services
    efi_status_t (__efiapi *allocate_pages)(enum efi_allocate_type alloc_type,
                                            enum efi_memory_type memory_type,
                                            uint64_t pages,
                                            uint64_t *phys_addr);

    efi_status_t (__efiapi *free_pages)(uint64_t phys_addr, uint64_t pages);

    efi_status_t (__efiapi *get_memory_map)(uint64_t *memory_map_size,
                                            struct efi_memory_desc *memory_map,
                                            uint64_t *map_key,
                                            uint64_t *descriptor_size,
                                            uint32_t *descriptor_version);

    efi_status_t (__efiapi *allocate_pool)(enum efi_memory_type pool_type, uint64_t size, void **buffer);
    efi_status_t (__efiapi *free_pool)(void *memory);

    // Event & Timer Services
    efi_status_t (__efiapi *create_event)(uint32_t event_type,
                                          uint64_t notify_tpl,
                                          efi_event_notify_t notify_function,
                                          void *notify_context,
                                          efi_event_t *event);
    efi_status_t (__efiapi *set_timer)(efi_event_t event,
                                       enum efi_timer_delay type,
                                       uint64_t trigger_time);
    efi_status_t (__efiapi *wait_for_event)(uint64_t num_events,  // number of events in event array
                                            efi_event_t *events,  // event array
                                            uint64_t *idx);
    efi_status_t (__efiapi *signal_event)(efi_event_t event);
    efi_status_t (__efiapi *close_event)(efi_event_t event);
    efi_status_t (__efiapi *check_event)(efi_event_t event);

    // Protocol Handler Services
    efi_status_t (__efiapi *install_protocol_interface)(efi_handle_t *handle,
                                                        const efi_guid_t *protocol,
                                                        enum efi_interface_type interface_type, // must always be 0
                                                        void *interface);

    efi_status_t (__efiapi *uninstall_protocol_interface)(efi_handle_t handle,
                                                          const efi_guid_t *protocol,
                                                          void *interface);

    efi_status_t (__efiapi *reinstall_protocol_interface)(efi_handle_t handle,
                                                          const efi_guid_t *protocol,
                                                          void *old_interface,
                                                          void *new_interface);

    efi_status_t (__efiapi *handle_protocol)(efi_handle_t handle,
                                             const efi_guid_t *protocol,
                                             void **interface);
    void*        Reserved;                            // EFI 1.0+
    void*        RegisterProtocolNotify;              // EFI  1.0+  // Requires Event & Timer Services
    efi_status_t (__efiapi *locate_handle)(enum efi_locate_search_type,
                                           const efi_guid_t *protocol,
                                           void *search_key,
                                           uint64_t *num_handles,
                                           efi_handle_t *handles);   // must be free'd after
    void*        LocateDevicePath;                    // EFI 1.0+
    void*        InstallConfigurationTable;           // EFI 1.0+

    //
    // Image Services
    void*        LoadImage;        // EFI 1.0+
    void*        StartImage;       // EFI 1.0+
    efi_status_t (__efiapi *exit)(efi_handle_t handle,
                                  efi_status_t status, 
                                  uint64_t exit_data_size, 
                                  void *exit_data);
    void*        UnloadImage;      // EFI 1.0+
    void*        ExitBootServices; // EFI 1.0+

    //
    // Miscellaneous Services
    void*        GetNextMonotonicCount; // EFI 1.0+
    efi_status_t (__efiapi *stall)(uint64_t usecs);
    efi_status_t (__efiapi *set_watchdog_timer)(uint64_t timeout,
                                                uint64_t watchdog_code, // watchdog timer code
                                                uint64_t data_size,     // size of data (in bytes)
                                                const uint16_t *data);  // null-terminated string to indicate watchdog event

    //
    // DriverSupport Services
    void*        ConnectController;     // EFI 1.1
    void*        DisconnectController;  // EFI 1.1+

    // Open and Close Protocol Services
    efi_status_t (__efiapi *open_protocol)(efi_handle_t handle,
                                           const efi_guid_t *protocol,
                                           void **interface,
                                           efi_handle_t agent,
                                           efi_handle_t controller,
                                           uint32_t attributes);
    efi_status_t (__efiapi *close_protocol)(efi_handle_t handle,
                                            const efi_guid_t *protocol,
                                            efi_handle_t agent,
                                            efi_handle_t controller);
    efi_status_t (__efiapi *open_protocol_information)(efi_handle_t handle,
                                                       const efi_guid_t *guid,
                                                       struct efi_open_protocol_information_entry **entries,
                                                       uint64_t *num_entries);

    // Library Services
    void*        ProtocolsPerHandle;     // EFI 1.1+
    efi_status_t (__efiapi *locate_handle_buffer)(enum efi_locate_search_type,
                                                  const efi_guid_t *protocol,
                                                  void *search_key,
                                                  uint64_t *num_handles,  // number of handles returned
                                                  efi_handle_t **handles); // must call BootServices.FreePool() when buffer is no longer required
    efi_status_t (__efiapi *locate_protocol)(const efi_guid_t *protocol,
                                             void *registration_key, // NULL means ignore
                                             void **interface);
    void*        InstallMultipleProtocolInterfaces;    // EFI 1.1+
    void*        UninstallMultipleProtocolInterfaces;   // EFI 1.1+*

    // 32-bit CRC Services
    efi_status_t (__efiapi *calculate_crc32)(const void *data,
                                             uint64_t data_size,
                                             uint32_t *crc32);

    // Miscellaneous Services
    void (__efiapi *copy_mem)(void *destination, const void* source, uint64_t length);
    void (__efiapi *set_mem)(void *buffer, uint64_t size, uint8_t value);
    efi_status_t (__efiapi *create_event_ex)(uint32_t event_type, 
                                             uint64_t notify_tpl,
                                             efi_event_notify_t notify_function,
                                             void *notify_context,
                                             const efi_guid_t *event_group,
                                             efi_event_t *event);
};


#endif
