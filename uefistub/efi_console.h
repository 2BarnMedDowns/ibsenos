#ifndef __IBSENOS_UEFISTUB_CONSOLE_H__
#define __IBSENOS_UEFISTUB_CONSOLE_H__

/*
 * Implemented following the UEFI 2.11 specification
 * See: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include <efi.h>
#include <efistub.h>

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


#endif
