#ifndef __IBSENOS_UEFISTUB_CONSOLE_H__
#define __IBSENOS_UEFISTUB_CONSOLE_H__

/*
 * Implemented following the UEFI 2.11 specification
 * See: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include <efi.h>


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
 * EFI simple text output protocol
 * See section 12.4.1
 */
struct efi_simple_text_output_protocol
{
    efi_status_t (__efiapi *reset)(const struct efi_simple_text_output_protocol*, 
                                   uint8_t extended_verification);

    efi_status_t (__efiapi *output_string)(const struct efi_simple_text_output_protocol*,
                                           const uint16_t *string);
    
    efi_status_t (__efiapi *test_string)(const struct efi_simple_text_output_protocol*,
                                         uint16_t *string);
    
    efi_status_t (__efiapi *query_mode)(const struct efi_simple_text_output_protocol*,
                                        uint64_t mode_number,
                                        uint64_t *columns,
                                        uint64_t *rows);

    efi_status_t (__efiapi *set_mode)(const struct efi_simple_text_output_protocol*,
                                      uint64_t mode);

    efi_status_t (__efiapi *set_attribute)(const struct efi_simple_text_output_protocol*,
                                           uint64_t attribute);

    efi_status_t (__efiapi *clear_screen)(const struct efi_simple_text_output_protocol*);

    efi_status_t (__efiapi *set_cursor)(const struct efi_simple_text_output_protocol*,
                                        uint64_t column,
                                        uint64_t row);

    efi_status_t (__efiapi *enable_cursor)(const struct efi_simple_text_output_protocol*,
                                           uint8_t visible);
};


/*
 * Write wide-char string to the console.
 */
void efi_char16_puts(const uint16_t *str);


/*
 * Write an UTF-8 encoded string to the console.
 */
void efi_puts(const char *str);


/*
 * Write an usigned 64-bit integer as decimal to console.
 */
void efi_putd(uint64_t value);


/*
 * Write an unsigned 64-bit integer as hexadecimal to console.
 */
void efi_puth(uint64_t value);


/*
 * Write an unsigned 64-bit integer as hexadecimal to console,
 * with leading zeros.
 */
void efi_put0h(uint64_t value);


void efi_console_reset(void);
void efi_console_clear_screen(void);
void efi_console_color(int color);
void efi_console_restore(void);

#endif
