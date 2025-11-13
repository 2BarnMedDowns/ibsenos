#include "uefi.h"
#include "systbl.h"
#include "console.h"


efi_status_t EFIAPI efi_main(void *, efi_systbl_t *systbl)
{
    systbl->conout->reset(systbl->conout, 0); // TODO: should be clearscreen instead
    systbl->conout->output_string(systbl->conout, L"Hello, world!\n\r");

    while (1);
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
