#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* st)
{
    st->ConOut->OutputString(st->ConOut, L"Hello World!\n");
    while(1);

    return EFI_SUCCESS;
}
