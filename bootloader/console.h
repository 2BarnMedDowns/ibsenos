#ifndef __UEFI_CONSOLE_H__
#define __UEFI_CONSOLE_H__

#include <stddef.h>
#include <stdint.h>
#include "uefi.h"


/*
 * UEFI 2.11 spec, 12.4.1
 * https://uefi.org/specs/UEFI/2.11/12_Protocols_Console_Support.html#efi-simple-text-output-protocol
 */
#define EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL_GUID \
{0x387477c2, 0x69c7, 0x11d2, {0x8e,0x39,0x00,0xa0,0xc9,0x69,0x72,0x3b}}


/* Forward declaration */
struct efi_simple_text_output_protocol;


struct efi_simple_text_output_protocol
{
    efi_status_t (EFIAPI *reset)(
        const struct efi_simple_text_output_protocol *this, 
        uint8_t extended_verification
    );
    efi_status_t (EFIAPI *output_string)(
        const struct efi_simple_text_output_protocol *this, 
        const wchar_t *string
    );
    // TODO
};

#endif
