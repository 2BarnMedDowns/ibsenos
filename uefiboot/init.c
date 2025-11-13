#include <efi.h>


static struct efi_system_table *ST = NULL;


static void print(efi_char16_t *str)
{
    efi_simple_text_output_protocol_t *proto = (efi_simple_text_output_protocol_t*) ST->con_out;
    proto->output_string(proto, str);
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

    print(L"UEFI firmware vendor: ");
    print((efi_char16_t*) systab->fw_vendor);
    print(L"\r\n");

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
