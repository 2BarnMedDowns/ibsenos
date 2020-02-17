#include <efi.h>
#include <efilib.h>


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImgHandle, EFI_SYSTEM_TABLE* SysTbl)
{
    EFI_STATUS status;

    status = SysTbl->ConOut->ClearScreen(SysTbl->ConOut);
    if (EFI_ERROR(status)) {
        SysTbl->ConOut->OutputString(SysTbl->ConOut, L"ERROR: Failed to clear the screen\n\r");
        return status;
    }

    SysTbl->ConOut->OutputString(SysTbl->ConOut, L"Please wait while preparing OS\n\r");

    /* TODO: ifdef DEBUG, disable UEFI watchdog timer */


    while (1);
    return EFI_SUCCESS;
}
