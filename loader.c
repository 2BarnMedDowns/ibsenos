#include <efi.h>
//#include <efilib.h>


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImgHandle, EFI_SYSTEM_TABLE* SysTbl)
{
    EFI_STATUS status;

    status = SysTbl->ConOut->ClearScreen(SysTbl->ConOut);
    if (EFI_ERROR(status)) {
        SysTbl->ConOut->OutputString(SysTbl->ConOut, L"ERROR: Failed to clear the screen\n\r");
        return status;
    }

    SysTbl->ConOut->OutputString(SysTbl->ConOut, L"Please wait while preparing OS\n\r");

#ifdef DEBUG
    SysTbl->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
#endif

    while (1);
    return EFI_SUCCESS;
}
