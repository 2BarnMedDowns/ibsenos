#ifndef _EFI_BOOT_SERVICES
#define _EFI_BOOT_SERVICES

#define IN
#define OUT
#define OPTIONAL

// https://codebrowser.dev/llvm/libc/include/llvm-libc-types/EFI_TPL.h.html
typedef size_t EFI_TPL;

// https://doxygen.reactos.org/df/dd2/UefiBaseType_8h.html#a5f446f35c9f907a4f1cf22e794ba338d
typedef uint32_t UINTN;
typedef UINTN EFI_STATUS;

typedef uint64_t EFI_PHYSICAL_ADDRESS;

// https://uefi.org/specs/UEFI/2.11/07_Services_Boot_Services.html#efi-boot-services-raisetpl
// Section 7.1.8
typedef EFI_TPL (*EFI_RAISE_TPL) (IN EFI_TPL NewTpl);

// Section 7.1.9
typedef void (*EFI_RESTORE_TPL) (IN EFI_TPL OldTpl);

// Section 7.2.1
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

typedef EFI_STATUS (*EFI_ALLOCATE_PAGES) (
   IN EFI_ALLOCATE_TYPE                   Type,
   IN EFI_MEMORY_TYPE                     MemoryType,
   IN UINTN                               Pages,
   IN OUT EFI_PHYSICAL_ADDRESS            *Memory
);




#endif // _EFI_BOOT_SERVICES
