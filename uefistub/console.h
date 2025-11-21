#ifndef __IBSENOS_UEFISTUB_CONSOLE_H__
#define __IBSENOS_UEFISTUB_CONSOLE_H__

/*
 * Implemented following the UEFI 2.11 specification
 * See: https://uefi.org/specs/UEFI/2.11/index.html
 */

#include <efi.h>
#include <boot.h>


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


struct efi_input_key
{
    uint16_t scan_code;
    uint16_t unicode_char;
};


/*
 * 12.2.3 EFI simple text input protocol
 */
struct efi_simple_text_input_protocol
{
    efi_status_t (__efiapi *reset)(const struct efi_simple_text_input_protocol*);

    efi_status_t (__efiapi *read_keystroke)(const struct efi_simple_text_input_protocol*,
                                            struct efi_input_key *key);

    efi_event_t *wait_for_key;  // event to wait for using boot services' wait for event
};


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
 * EFI graphics output protocol
 * See section 12.9.1
 */
enum efi_graphics_pixel_format
{
    PIXEL_RGB_RESERVED_8BIT_PER_COLOR = 0,
    PIXEL_BGR_RESERVED_8BIT_PER_COLOR = 1,
    PIXEL_BIT_MASK = 2,
    PIXEL_BLT_ONLY = 3,
    PIXEL_FORMAT_MAX
};

struct efi_pixel_bitmask
{
    uint32_t red_mask;
    uint32_t green_mask;
    uint32_t blue_mask;
    uint32_t reserved_mask;
};

struct efi_graphics_output_mode_info
{
    uint32_t version;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    enum efi_graphics_pixel_format pixel_format;
    struct efi_pixel_bitmask pixel_information;
    uint32_t pixels_per_scan_line;
};

struct efi_graphics_output_blt_pixel
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t reserved;
};

enum efi_graphics_output_blt_operation {
    EFI_BLT_VIDEO_FILL = 0,
    EFI_BLT_VIDEO_TO_BLT_BUFFER = 1,
    EFI_BLT_BUFFER_TO_VIDEO = 2,
    EFI_BLT_VIDEO_TO_VIDEO = 3,
    EFI_GRAPHICS_OUTPUT_BLT_OPERATION_MAX
};

/*
 * Base address of graphics linear frame buffer.
 * Info contains information required to allow software to draw directly to the frame buffer without using Blt().
 * Offset zero in FrameBufferBase represents the upper left pixel of the display.
 */
struct efi_graphics_output_protocol_mode
{
    uint32_t max_mode;
    uint32_t mode;
    const struct efi_graphics_output_mode_info *info;
    uint64_t size_of_info;
    uint64_t frame_buffer_base; // EFI_PHYSICAL_ADDRESS -> typedef UINT64 EFI_PHYSICAL_ADDRESS
    uint64_t frame_buffer_size;
};


struct efi_graphics_output_protocol
{
    efi_status_t (__efiapi *query_mode)(const struct efi_graphics_output_protocol*,
                                        uint32_t mode_number,
                                        uint64_t *info_size,
                                        struct efi_graphics_output_mode_info **info);

    efi_status_t (__efiapi *set_mode)(const struct efi_graphics_output_protocol*,
                                      uint32_t mode_number);

    efi_status_t (__efiapi *blt)(const struct efi_graphics_output_protocol*,
                                 struct efi_graphics_output_blt_pixel *blt_buffer,
                                 enum efi_graphics_output_blt_operation blt_operation,
                                 uint64_t source_x,
                                 uint64_t source_y,
                                 uint64_t destination_x,
                                 uint64_t destination_y,
                                 uint64_t width,
                                 uint64_t height,
                                 uint64_t delta);

    const struct efi_graphics_output_protocol_mode *mode;
};


/*
 * Write wide-char string to the console.
 */
void efi_char16_puts(const uint16_t *str);


/*
 * Write a wide-char character to the console.
 */
void efi_char16_putc(uint16_t c);


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


efi_status_t efi_wait_for_key(uint32_t usecs, struct efi_input_key *key);


/*
 * Setup the graphics output protocol.
 */
efi_status_t efi_setup_gop(struct screen_info *si);


#endif
