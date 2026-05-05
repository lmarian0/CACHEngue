#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);
    ST->ConOut->ClearScreen(ST->ConOut);
    Print(L"Hello World!\n");
    for(;;) {}
    return EFI_SUCCESS;
}