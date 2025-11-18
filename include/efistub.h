/*
 * EFI stub functions.
 */
#ifndef __IBSENOS_EFISTUB_H__
#define __IBSENOS_EFISTUB_H__

#include <inttypes.h>



#define EFI_CONSOLE_BLACK           0x00
#define EFI_CONSOLE_BLUE            0x01
#define EFI_CONSOLE_GREEN           0x02
#define EFI_CONSOLE_CYAN            0x03
#define EFI_CONSOLE_RED             0x04
#define EFI_CONSOLE_MAGENTA         0x05
#define EFI_CONSOLE_BROWN           0x06
#define EFI_CONSOLE_GRAY            0x07
#define EFI_CONSOLE_DARKGRAY        0x08
#define EFI_CONSOLE_BRIGHTBLUE      0x09
#define EFI_CONSOLE_BRIGHTGREEN     0x0a
#define EFI_CONSOLE_BRIGHTCYAN      0x0b
#define EFI_CONSOLE_BRIGHTRED       0x0c
#define EFI_CONSOLE_BRIGHTMAGENTA   0x0d
#define EFI_CONSOLE_YELLOW          0x0e
#define EFI_CONSOLE_WHITE           0x0f


#define EFI_CONSOLE_COLOR(foreground, background) \
    ( ((foreground) & 0xf) | (((background) & 0x7) << 4) )


/*
 * Write wide-char string to the console.
 */
void efi_char16_puts(const uint16_t *str);


/*
 * Write an UTF-8 encoded string to the console.
 */
void efi_puts(const char *str);


/*
 * Allocate a range of memory (pages) below the address given by below_addr
 */
efi_status_t efi_allocate_memory(uint64_t size, 
                                 uint64_t *phys_addr,
                                 uint64_t below_addr);


/*
 * Free memory range.
 */
void efi_free_memory(uint64_t size, uint64_t phys_addr);


#endif
