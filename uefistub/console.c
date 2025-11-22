#include "systab.h"
#include "console.h"
#include "memory.h"
#include <efi.h>
#include <inttypes.h>
#include <cdefs.h>
#include <string.h>


/*
 * Convert UF8-8 octets to UTF-32
 * Blatently ripped off from the Linux kernel's UEFI stub
 */
static uint32_t utf8_to_utf32(const uint8_t **s8)
{
    uint32_t c32;
    uint8_t c0, cx;
    size_t clen = 0;

    c0 = cx = *(*s8)++;

    // The position of the most significant 0 bit
    // is the length of a multi-octet string
    for (; cx & 0x80; ++clen)
        cx <<= 1;

    // If the 0 bit is in position 8, this is a valid
    // single-octet encoding. If it is in position 7 or
    // positions 1-3, the encoding is invalid.
    if (clen < 2 || clen > 4) {
        return c0;
    }

    // Get the bits from the first octet
    c32 = cx >> clen--;
    for (size_t i = 0; i < clen; ++i) {
        // trailing octets must have 10 in most significant bits
        cx = (*s8)[i] ^ 0x80;
        if (cx & 0xc0) {
            return c0;
        }
        c32 = (c32 << 6) | cx;
    }

    // Check for validity:
    // - within unicode range
    // - must not be a surrogate
    // - must be encoded using the correct number of octets
    if (c32 > 0x10ffff || (c32 & 0xf800) == 0xd800 || 
            clen != (size_t) ((c32 >= 0x80) + (c32 >= 0x800) + (c32 >= 0x10000))) {
        return c0;
    }

    *s8 += clen;
    return c32;
}


void efi_puts(const char *str)
{
    uint16_t buf[128];
    size_t pos = 0;
    size_t lim = ARRAY_SIZE(buf);

    const uint8_t *s8 = (const uint8_t*) str;
    uint32_t c32;

    while (*s8 != '\0') {
        if (*s8 == '\n') {
            buf[pos++] = L'\r';
        }
        c32 = utf8_to_utf32(&s8);
        if (c32 < 0x10000) {
            // characters in plane 0 use a single word
            buf[pos++] = c32;
        } else {
            // characters in other planes encode into a surrogate pair
            buf[pos++] = (0xd800 - (0x10000 >> 10)) + (c32 >> 10);
            buf[pos++] = 0xdc00 + (c32 & 0x3ff);
        }
        if (*s8 == '\0' || pos >= lim - 2) {
            buf[pos] = L'\0';
            efi_char16_puts(buf);
            pos = 0;
        }
    }
}


void efi_char16_puts(const uint16_t *str)
{
    const struct efi_simple_text_output_protocol *conout =
        (const struct efi_simple_text_output_protocol*) ST->console_out;

    conout->output_string(conout, str);
}


void efi_char16_putc(uint16_t c)
{
    uint16_t buf[2] = {c, '\0'};
    efi_char16_puts(buf);
}


void efi_putd(uint64_t value)
{
    char buf[21];
    u64tostr(value, buf, 10);
    efi_puts(buf);
}



void efi_puth(uint64_t value)
{
    char buf[17];
    u64tostr(value, buf, 16);
    efi_puts(buf);
}


void efi_put0h(uint64_t value, size_t digits)
{
    char buf[17];
    size_t n = u64tostr(value, buf, 16);
    strrev(buf);
    memset(&buf[n], '0', 15 - n);
    buf[16] = '\0';
    strrev(buf);

    if (n > digits) {
        digits = n;
    }

    efi_puts(&buf[15-digits]);
}


void efi_console_reset(void)
{
    const struct efi_simple_text_output_protocol *conout =
        (const struct efi_simple_text_output_protocol*) ST->console_out;

    const struct efi_simple_text_input_protocol *conin =
        (const struct efi_simple_text_input_protocol*) ST->console_in;

    conin->reset(conin);
    conout->reset(conout, 0);

    // Identify console with largest rows * columns
    // mode 0 which is 80x25 is default, so we can start looking from mode 1
    uint64_t columns = 80; 
    uint64_t rows = 25;
    uint64_t mode = 0;

    for (uint64_t i = 1; i < 64; ++i) {
        uint64_t x = 0;
        uint64_t y = 0;

        efi_status_t status = conout->query_mode(conout, i, &x, &y);
        if (status != EFI_SUCCESS) {
            continue;
        }

        if (y * x > rows * columns && x >= columns) {
            mode = i;
            columns = x;
            rows = y;
        }
    }

    conout->set_mode(conout, mode);
}


void efi_console_clear_screen(void)
{
    const struct efi_simple_text_output_protocol *conout =
        (const struct efi_simple_text_output_protocol*) ST->console_out;

    conout->set_attribute(conout, EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));
    conout->clear_screen(conout);
    conout->set_cursor(conout, 0, 0);
}


void efi_console_color(int color)
{
    uint64_t attr = EFI_CONSOLE_COLOR(color, EFI_CONSOLE_BLACK);

    const struct efi_simple_text_output_protocol *conout =
        (const struct efi_simple_text_output_protocol*) ST->console_out;

    conout->set_attribute(conout, attr);
}


void efi_console_restore(void)
{
    efi_console_color(EFI_CONSOLE_GRAY);
}


efi_status_t efi_wait_for_key(uint32_t usecs, struct efi_input_key *key)
{
    efi_event_t events[2] = {NULL, NULL};
    uint64_t event_idx = 3;

    const struct efi_boot_services *bs =
        (const struct efi_boot_services*) ST->boot_services;

    const struct efi_simple_text_input_protocol *conin =
        (const struct efi_simple_text_input_protocol*) ST->console_in;
    if (conin == NULL) {
        return EFI_UNSUPPORTED;
    }

    events[0] = conin->wait_for_key;

    efi_status_t status = bs->create_event(EFI_EVT_TIMER, 0, NULL, NULL, &events[1]);
    if (status != EFI_SUCCESS) {
        return status;
    }

    // BootServices.SetTimer takes the time in 100 ns units
    status = bs->set_timer(events[1], EFI_TIMER_RELATIVE, (uint64_t) 10 * usecs);
    if (status != EFI_SUCCESS) {
        return status;
    }

    status = bs->wait_for_event(2, events, &event_idx);
    if (status == EFI_SUCCESS) {
        if (event_idx == 0) {
            status = conin->read_keystroke(conin, key);
        } else {
            status = EFI_TIMEOUT;
        }
    }

    bs->close_event(events[1]);
    return status;
}
