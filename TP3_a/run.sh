qemu-system-x86_64 -m 512 \
  -bios /usr/share/ovmf/OVMF.fd \
  -drive file=fat:rw:hdd,format=raw,media=disk \
  -net none \
  -display gtk \
  -s -S