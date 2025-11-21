#ifndef __IBSENOS_BOOT_H__
#define __IBSENOS_BOOT_H__

#include <compiler.h>
#include <inttypes.h>


struct screen_info 
{
    uint16_t    lfb_width;      // linear framebuffer width
    uint16_t    lfb_height;     // linear framebuffer height
    uint64_t    lfb_base;       // base address of linear framebuffer
    uint16_t    lfb_depth;
    uint16_t    lfb_linelength; // pixels per scan line
    uint16_t    lfb_size;
    uint8_t     red_size;
    uint8_t     red_pos;
    uint8_t     green_size;
    uint8_t     green_pos;
    uint8_t     blue_size;
    uint8_t     blue_pos;
    uint8_t     reserved_size;
    uint8_t     reserved_pos;

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
