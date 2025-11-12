#ifndef __EFI_H__
#define __EFI_H__

#include <stdint.h>

#define EFIAPI __attribute__((ms_abi))

typedef enum {
    EFI_SUCCESS = 0,
    EFI_ERROR = 1
} efi_status_t;


#endif
