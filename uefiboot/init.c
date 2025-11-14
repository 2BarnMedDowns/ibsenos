#include <efi.h>


static struct efi_system_table *ST = NULL;


static void set_color(uint8_t fg, uint8_t bg)
{
    efi_simple_text_output_protocol_t *proto =
        (efi_simple_text_output_protocol_t*) ST->con_out;
    proto->set_attr(proto, EFI_CONSOLE_FG(fg) | EFI_CONSOLE_BG(bg));
}


static void print(uint16_t *str)
{
    efi_simple_text_output_protocol_t *proto = 
        (efi_simple_text_output_protocol_t*) ST->con_out;
    proto->output_string(proto, str);
}


static void print_uint(uint64_t n)
{
    uint16_t buffer[21] = {0};
    uint8_t len = 21;

    efi_simple_text_output_protocol_t *proto = 
        (efi_simple_text_output_protocol_t*) ST->con_out;

    buffer[len--] = 0;
    if (n == 0) {
        buffer[len--] = L'0';
    } else {
        while (n > 0) {
            buffer[len--] = L'0' + (n % 10);
            n /= 10;
        }
    }

    proto->output_string(proto, &buffer[len+1]);
}


/*
 * EFI image entry point
 * https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html#uefi-image-entry-point
 */
efi_status_t __efiapi uefi_init(void *, struct efi_system_table *systab)
{
    ST = NULL;

    if (systab->hdr.signature != EFI_SYSTEM_TABLE_SIGNATURE) {
        return EFI_INVALID_PARAMETER;
    }

    ST = systab;

    uint32_t fwrev_major = (systab->fw_revision >> 16);
    uint32_t fwrev_minor = (systab->fw_revision & 0xffff) / 10;
    uint32_t fwrev_tertiary = systab->fw_revision % 10;

    set_color(0xf, 0x1);

    print(L"UEFI firmware: ");
    print((uint16_t*) systab->fw_vendor);
    print(L" (revision ");
    print_uint(fwrev_major);
    print(L".");
    print_uint(fwrev_minor);
    print(L".");
    print_uint(fwrev_tertiary);
    print(L")\r\n");

    //print(L"\x2588 \x250c \x2514 \x2591");

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
