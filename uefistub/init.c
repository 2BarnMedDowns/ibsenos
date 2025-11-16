#include "efi.h"
#include "efi_console.h"
#include <inttypes.h>
#include <string.h>


const struct efi_system_table *ST = NULL;


//static void draw_box(uint64_t x, uint64_t y, uint64_t w, uint64_t h)
//{
//    uint16_t line[w];
//    line[w-1] = 0;
//
//    console_out->set_cursor(console_out, x, y);
//    line[0] = 0x250c;
//    for (uint16_t c = 1; c < w-1; ++c) {
//        line[c] = 0x2500;
//    }
//    line[w-1] = 0x2510;
//    efi_char16_puts(line);
//
//    for (uint16_t r = y+1; r < y+h-1; ++r) {
//        console_out->set_cursor(console_out, x, r);
//        efi_char16_puts(L"\x2502");
//        console_out->set_cursor(console_out, x+w-1, r);
//        efi_char16_puts(L"\x2502");
//    }
//
//    console_out->set_cursor(console_out, x, y+h-1);
//    line[0] = 0x2514;
//    for (uint16_t c = 1; c < w-1; ++c) {
//        line[c] = 0x2500;
//    }
//    line[w-1] = 0x2518;
//    efi_char16_puts(line);
//}


/*
 * EFI image entry point
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#uefi-image-entry-point
 */
efi_status_t __efiapi uefi_entry(void *, struct efi_system_table *systab)
{
    efi_status_t status;
    ST = NULL;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        return EFI_INVALID_PARAMETER;
    }

    ST = systab;
    const struct efi_simple_text_output_protocol * console_out =
        (const struct efi_simple_text_output_protocol*) systab->console_out;

    // Find console mode with largest rows * columns
    uint64_t console_columns = 80;
    uint64_t console_rows = 25;
    uint64_t console_mode = 0;
    for (uint64_t i = 1; i  < 16; ++i) {
        uint64_t columns = 0;
        uint64_t rows = 0;

        status = console_out->query_mode(console_out, i, &columns, &rows);
        if (status == EFI_SUCCESS && rows * columns > console_rows * console_columns) {
            console_mode = i;
            console_columns = columns;
            console_rows = rows;
        }
    }

    status = console_out->set_mode(console_out, console_mode);
    if (status != EFI_SUCCESS) {
        return EFI_LOAD_ERROR;
    }

    uint32_t uefirev = systab->hdr.revision;
    uint32_t uefi_major = (uefirev >> 16);
    uint32_t uefi_minor = (uefirev & 0xffff) / 10;
    uint32_t uefi_tertiary = uefirev % 10;

    uint32_t fwrev_major = (systab->fw_revision >> 16);
    uint32_t fwrev_minor = (systab->fw_revision & 0xffff);

    console_out->set_attribute(console_out, 
            EFI_CONSOLE_COLOR(EFI_CONSOLE_GRAY, EFI_CONSOLE_BLACK));
    console_out->clear_screen(console_out);

    //draw_box(2, 4, console_columns - 4, console_rows - 6);

    console_out->set_cursor(console_out, 0, 0);

    char buf[65];
    efi_puts("UEFI revision: ");
    u64tostr(uefi_major, buf, 0);
    efi_puts(buf);
    efi_puts(".");
    u64tostr(uefi_minor, buf, 0);
    efi_puts(buf);
    efi_puts(".");
    u64tostr(uefi_tertiary, buf, 0);
    efi_puts(buf);
    efi_puts("\n");

    efi_puts("Firmware vendor: ");
    efi_char16_puts((const uint16_t*) systab->fw_vendor);
    efi_puts("\n");

    efi_puts("Firmware revision: ");
    u64tostr(fwrev_major, buf, 0);
    efi_puts(buf);
    efi_puts(".");
    u64tostr(fwrev_minor, buf, 0);
    efi_puts(buf);
    efi_puts("\n");

    u64tostr((1ULL<<64ULL)-1, buf, 36);
    //u64tostr(0xdeadbeef, buf, 36);
    //u64tostr(123456789, buf, 36);
    //u64tostr(11, buf, 10);
    efi_puts(buf);

    // TODO: set up boot services and stuff here

    for (;;) {}
}

//
//static EFI_STATUS EFIAPI PrintLn(IN EFI_SYSTEM_TABLE * SysTbl, IN CHAR16 * String) 
//{
//    EFI_STATUS Status;
//   
//    Status = SysTbl->ConOut->OutputString(SysTbl->ConOut, String);
//    if (EFI_ERROR(Status)) {
//        return Status;
//    }
//
//    Status = SysTbl->ConOut->OutputString(SysTbl->ConOut, L"\n\r");
//    return Status;
//}
//
//
//EFI_STATUS EFIAPI Main(EFI_HANDLE ImgHandle, EFI_SYSTEM_TABLE* SysTbl)
//{
//    InitializeLib(ImgHandle, SysTbl);
//
//    EFI_STATUS Status;
//
//    Status = SysTbl->ConOut->ClearScreen(SysTbl->ConOut);
//    if (EFI_ERROR(Status)) {
//        PrintLn(SysTbl, L"ERROR: Failed to clear the screen");
//    }
//
//    //SysTbl->ConOut->OutputString(SysTbl->ConOut, L"Please wait while preparing OS\n\r");
//
//#ifdef DEBUG
//    SysTbl->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
//#endif
//
//    EFI_TIME now;
//    Status = SysTbl->RuntimeServices->GetTime(&now, NULL);
//    if (EFI_ERROR(Status)) {
//        PrintLn(SysTbl, L"Failed to get system time, something is wrong...");
//        return Status;
//    }
//
//    while (1);
//    return EFI_SUCCESS;
//}
