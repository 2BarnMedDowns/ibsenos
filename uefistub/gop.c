#include <cdefs.h>
#include "systab.h"
#include "console.h"
#include "memory.h"
#include <efi.h>
#include <boot.h>
#include <bitops.h>
#include <string.h>


static struct efi_graphics_output_protocol * find_gop(uint64_t num_handles, const efi_handle_t handles[])
{
    struct efi_graphics_output_protocol *first_gop = NULL;

    for (uint64_t i = 0; i < num_handles; ++i) {
        efi_handle_t handle = handles[i];

        struct efi_graphics_output_protocol *gop = NULL;
        const struct efi_graphics_output_protocol_mode *mode = NULL;
        const struct efi_graphics_output_mode_info *info = NULL;
        void *dummy = NULL;

        efi_status_t status = BS->handle_protocol(
                handle,
                &EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 
                (void**) &gop);
        if (status != EFI_SUCCESS) {
            continue;
        }

        mode = gop->mode;
        info = mode->info;

        if (info->pixel_format == PIXEL_BLT_ONLY ||
                info->pixel_format >= PIXEL_FORMAT_MAX) {
            continue;
        }

        status = BS->handle_protocol(
                handle,
                &EFI_CONSOLE_OUT_DEVICE_GUID, 
                &dummy);
        if (status == EFI_SUCCESS) {
            return gop;
        }

        if (first_gop == NULL) {
            first_gop = gop;
        }
    }
    
    return first_gop;
}


static uint8_t pixel_bpp(enum efi_graphics_pixel_format pixel_format, struct efi_pixel_bitmask pixel_info)
{
    if (pixel_format == PIXEL_BIT_MASK) {
        uint32_t mask = pixel_info.red_mask | pixel_info.green_mask | pixel_info.blue_mask | pixel_info.reserved_mask;
        if (mask == 0) {
            return 0;
        }
        return __fls(mask) - __ffs(mask) + 1;
    } else {
        return 32;
    }
}


static void find_bits(uint32_t mask, uint8_t *position, uint8_t *size)
{
    if (mask == 0) {
        *position = 0;
        *size = 0;
        return;
    }

    // EFI specification guarantees that the set bits are contiguous
    *position = __ffs(mask);
    *size = __fls(mask) - *position + 1;
}


static uint32_t choose_mode(struct efi_graphics_output_protocol *gop,
                            bool (*match)(const struct efi_graphics_output_mode_info*, uint32_t, void*),
                            void *ctx)
{
    const struct efi_graphics_output_protocol_mode *mode = gop->mode;
    uint32_t max_mode = mode->max_mode;
    uint32_t current_mode = mode->mode;

    for (uint32_t m = 0; m < max_mode; ++m) {
        struct efi_graphics_output_mode_info *info = NULL;
        uint64_t info_size = 0;
        efi_status_t status;

        status = gop->query_mode(gop, m, &info_size, &info);
        if (status != EFI_SUCCESS) {
            continue;
        }

        bool matched = match(info, m, ctx);
        efi_free_buffer(info);
        if (matched) {
            return m;
        }
    }

    return current_mode;
}


#ifdef DEBUG
static bool print_list_entry(const struct efi_graphics_output_mode_info *info, uint32_t mode, void *ctx)
{
    struct efi_pixel_bitmask pi = info->pixel_information;
    uint32_t current_mode = *((uint32_t*) ctx);
    enum efi_graphics_pixel_format pf = info->pixel_format;
    const char *str;
    uint8_t depth = 0;

    bool valid = !(pf == PIXEL_BLT_ONLY || pf >= PIXEL_FORMAT_MAX);
    switch (pf) {
        case PIXEL_RGB_RESERVED_8BIT_PER_COLOR:
            str = "rgb";
            break;

        case PIXEL_BGR_RESERVED_8BIT_PER_COLOR:
            str = "bgr";
            break;

        case PIXEL_BIT_MASK:
            str = "";
            depth = pixel_bpp(pf, pi);
            break;

        default:
            str = "xxx";
            break;
    }

    efi_puts("Mode ");

    char buf[4];
    size_t digits = u64tostr(mode, buf, 10);
    strrev(buf);
    memset(&buf[digits], '0', 4-digits);
    buf[3] = '\0';
    strrev(buf);
    efi_puts(buf);
    efi_puts(" ");

    uint16_t c = (mode == current_mode ? L'*' : L' ');
    efi_char16_putc(c);
    c = !valid ? L'-' : ' ';
    efi_char16_putc(c);

    efi_puts(": Resolution ");
    efi_putd(info->horizontal_resolution);
    efi_puts("x");
    efi_putd(info->vertical_resolution);
    efi_puts("-");
    efi_puts(str);
    efi_putd(depth);

    efi_puts("\n");
    return false;
}
#endif


#ifdef DEBUG
static uint32_t print_modes_list(struct efi_graphics_output_protocol *gop)
{
    const struct efi_graphics_output_protocol_mode *mode = gop->mode;
    uint32_t current_mode = mode->mode;
    uint32_t max_mode = mode->max_mode;
    struct efi_input_key key;

    efi_puts("Available graphics modes are 0-");
    efi_putd(max_mode - 1);
    efi_puts("\n");

    efi_puts("  * = current mode\n");
    efi_puts("  - = unusable mode\n");
    efi_puts("Will try to select mode with highest resolution and greatest depth\n");

    choose_mode(gop, print_list_entry, &current_mode);

    efi_puts("\nPress any key to continue or wait 10 seconds\n");
    efi_status_t status = efi_wait_for_key(10 * 1000000, &key);
    if (status != EFI_SUCCESS && status != EFI_TIMEOUT) {
        efi_puts("Unable to read key, continuing in 10 seconds\n");
        BS->stall(10 * 1000000);
    }

    return current_mode;
}
#endif


/*
 * Keep track of which mode has the highest resolution.
 */
struct match
{
    uint32_t mode;
    uint32_t area;
    uint8_t depth;
};


/*
 * Find screen mode with highest resolution.
 */
static bool highest_res(const struct efi_graphics_output_mode_info *info,
                        uint32_t mode,
                        void *ctx)
{
    uint32_t area = info->horizontal_resolution * info->vertical_resolution;
    struct efi_pixel_bitmask pi = info->pixel_information;
    enum efi_graphics_pixel_format pf = info->pixel_format;
    uint8_t depth = pixel_bpp(pf, pi);
    struct match *m = ctx;

    if (pf == PIXEL_BLT_ONLY || pf >= PIXEL_FORMAT_MAX) {
        return false;
    }

    if (area > m->area || (area == m->area && depth > m->depth)) {
        m->mode = mode;
        m->area = area;
        m->depth = depth;
    }

    return false;
}


static uint32_t choose_highest_res(struct efi_graphics_output_protocol *gop)
{
    struct match m = {};
    choose_mode(gop, highest_res, &m);
    return m.mode;
}


static void set_gop_mode(struct efi_graphics_output_protocol *gop, enum graphics_mode requested_mode)
{
    const struct efi_graphics_output_protocol_mode *mode = gop->mode;
    uint32_t current_mode = mode->mode;
    uint32_t new_mode = current_mode;

    switch (requested_mode) {
        case GRAPHICS_MODE_AUTO:
        case GRAPHICS_MODE_HIGHEST_RESOLUTION:
            new_mode = choose_highest_res(gop);
            break;

        default:
            efi_puts("ERROR: Unknown requested graphics mode\n");
            return;
    }

    if (new_mode == current_mode) {
#ifdef DEBUG
        print_modes_list(gop);
#endif
        return;
    }

    efi_status_t status = gop->set_mode(gop, new_mode);
    if (status != EFI_SUCCESS) {
        efi_puts("ERROR: Failed to set requested graphics mode\n");
    }

#ifdef DEBUG
    print_modes_list(gop);
#endif

}


efi_status_t efi_setup_gop(struct screen_info *si, enum graphics_mode requested_mode)
{
    efi_handle_t *handles = NULL;
    const struct efi_graphics_output_protocol_mode *mode = NULL;
    const struct efi_graphics_output_mode_info *info = NULL;
    struct efi_graphics_output_protocol *gop = NULL;
    uint64_t num = 0;
    efi_status_t status;

    status = BS->locate_handle_buffer(EFI_LOCATE_BY_PROTOCOL,
                                      &EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 
                                      NULL, 
                                      &num,
                                      &handles);
    if (status != EFI_SUCCESS) {
        return status;
    }

    gop = find_gop(num, handles);
    if (gop == NULL) {
        efi_free_buffer(handles);
        return EFI_NOT_FOUND;
    }

    set_gop_mode(gop, requested_mode);

    mode = gop->mode;
    info = mode->info;

    si->lfb_base = mode->frame_buffer_base;

    // Screen width and height
    si->lfb_width = info->horizontal_resolution;
    si->lfb_height = info->vertical_resolution;

    // Extract pixel information
    enum efi_graphics_pixel_format pf = info->pixel_format;
    struct efi_pixel_bitmask pi = info->pixel_information;

    if (pf == PIXEL_BIT_MASK) {
        find_bits(pi.red_mask, &si->red_pos, &si->red_size);
        find_bits(pi.green_mask, &si->green_pos, &si->green_size);
        find_bits(pi.blue_mask, &si->blue_pos, &si->blue_size);
        find_bits(pi.reserved_mask, &si->reserved_pos, &si->reserved_size);

        si->lfb_depth = si->red_size + si->green_size +
            si->blue_size + si->reserved_size;

        si->lfb_linelength = (info->pixels_per_scan_line * si->lfb_depth) / 8;

    } else {
        if (pf == PIXEL_RGB_RESERVED_8BIT_PER_COLOR) {
            si->red_pos = 0;
            si->blue_pos = 16;
        } else { // PIXEL_BGR_RESERVED_8BIT_PER_COLOR
            si->blue_pos = 0;
            si->red_pos = 16;
        }

        si->green_pos = 8;
        si->reserved_pos = 24;
        si->red_size = si->green_size = si->blue_size = si->reserved_size = 8;
        si->lfb_depth = 32;
        si->lfb_linelength = info->pixels_per_scan_line;
    }

    // Total size of framebuffer
    si->lfb_size = si->lfb_linelength * si->lfb_height;

    efi_free_buffer(handles);
    return EFI_SUCCESS;
}
