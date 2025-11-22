#ifndef __IBSENOS_BOOT_H__
#define __IBSENOS_BOOT_H__

#include <compiler.h>
#include <inttypes.h>


/*
 * Information about a linear frame buffer (LFB)
 */
struct screen_info 
{
    uint16_t    lfb_width;      // linear framebuffer width
    uint16_t    lfb_height;     // linear framebuffer height
    uint64_t    lfb_base;       // base address of linear framebuffer
    uint16_t    lfb_depth;      // color depth (r + g + b + reserved)
    uint16_t    lfb_linelength; // pixels per scan line
    uint16_t    lfb_size;       // total size (scan line length * height)
    uint8_t     red_size;       // number of bits used red
    uint8_t     red_pos;        // position of lowest bit in red
    uint8_t     green_size;     // number of bits used for green
    uint8_t     green_pos;      // position of lowest bit in green
    uint8_t     blue_size;      // number of bits used for blue
    uint8_t     blue_pos;       // position of lowest bit in blue
    uint8_t     reserved_size;
    uint8_t     reserved_pos;
} __packed;


/*
 * Some information from the UEFI firmware
 */
struct efi_info
{
    uint64_t efi_systab;        // address of the EFI system table
    uint64_t efi_memmap;        // address of the memory map from EFI
    uint64_t efi_memmap_size;   // total size of the memory map
    uint64_t efi_memdesc_size;  // size of each memory map descriptor
    uint64_t efi_memdesc_version; // memory map descriptor version (should be 1)
} __packed;


/*
 * Parameters from the boot loader to the kernel.
 */
struct boot_params
{
    struct screen_info screen_info;
    struct efi_info efi_info;
} __packed;


#endif
