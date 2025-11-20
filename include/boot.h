#ifndef __IBSENOS_BOOT_H__
#define __IBSENOS_BOOT_H__

#include <compiler.h>
#include <inttypes.h>


struct screen_info 
{

} __packed;


struct efi_info
{
    uint64_t efi_systab;
    uint64_t efi_memmap;
    uint64_t efi_memmap_size;
    uint64_t efi_memdesc_size;
    uint64_t efi_memdesc_version;
};



struct boot_params
{
    struct screen_info screen_info;
    struct efi_info efi_info;
} __packed;



#endif
