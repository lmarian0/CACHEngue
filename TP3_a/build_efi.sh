echo "Compilando..."
# 1. Compilar a codigo objeto
gcc -g -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -fpic -ffreestanding -fno-stack-protector -fno-strict-aliasing -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -Wall -c -o aplicacion.o aplicacion.c

# 2. Linkear
ld -shared -Bsymbolic -L/usr/lib -L/usr/lib/efi -T/usr/lib/elf_x86_64_efi.lds /usr/lib/crt0-efi-x86_64.o aplicacion.o -o aplicacion.so -lefi -lgnuefi

# 3. Convertir a ejecutable EFI
objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 aplicacion.so aplicacion.efi

echo "Desplegando al disco virtual..."
# 4. Crear la ruta por si no existe y copiar el archivo
mkdir -p hdd/EFI/BOOT
cp aplicacion.efi hdd/EFI/BOOT/BOOTX64.EFI

echo "¡Build completado con éxito!"