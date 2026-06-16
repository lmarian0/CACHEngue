#!/bin/bash
# =============================================================================
# setup_qemu.sh - Descarga y lanza QEMU emulando una Raspberry Pi
#
# Basado en la imagen Raspberry Pi OS Lite (32-bit, Bullseye)
# Adaptado para desarrollo de device drivers sin hardware real.
#
# Ejecutar en el HOST (PC de desarrollo), no en el target.
# =============================================================================

set -e

WORKDIR="$HOME/qemu-rpi"
mkdir -p "$WORKDIR"
cd "$WORKDIR"

echo "=============================================="
echo "  Setup QEMU para Raspberry Pi (ARM)"
echo "=============================================="

# ---------- 1. Instalar dependencias ----------
echo ""
echo "[1/5] Instalando dependencias..."
sudo apt update -q
sudo apt install -y \
    qemu-system-arm \
    qemu-utils \
    gcc-arm-linux-gnueabihf \
    wget \
    unzip \
    xz-utils

# ---------- 2. Descargar kernel e imagen para QEMU ----------
# Usamos el proyecto qemu-rpi-gpio o la imagen oficial de RPi OS
# Kernel y DTB pre-compilados para QEMU (versionsil del repo de dhruvvyas90)
echo ""
echo "[2/5] Descargando kernel y DTB para QEMU..."

KERNEL_URL="https://github.com/dhruvvyas90/qemu-rpi-kernel/raw/master/kernel-qemu-4.19.50-buster"
DTB_URL="https://github.com/dhruvvyas90/qemu-rpi-kernel/raw/master/versatile-pb-buster.dtb"

[ -f kernel-qemu ] || wget -q --show-progress -O kernel-qemu "$KERNEL_URL"
[ -f versatile-pb.dtb ] || wget -q --show-progress -O versatile-pb.dtb "$DTB_URL"

# ---------- 3. Descargar imagen Raspberry Pi OS ----------
echo ""
echo "[3/5] Descargando imagen Raspberry Pi OS Lite..."
echo "      (esto puede tardar varios minutos)"

IMG_URL="https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2021-05-28/2021-05-07-raspios-buster-armhf-lite.zip"
IMG_ZIP="raspios-lite.zip"
IMG_FILE="2021-05-07-raspios-buster-armhf-lite.img"

if [ ! -f "$IMG_FILE" ]; then
    wget -q --show-progress -O "$IMG_ZIP" "$IMG_URL"
    unzip -o "$IMG_ZIP"
    rm "$IMG_ZIP"
fi

# Expandir imagen para tener más espacio
echo ""
echo "[4/5] Expandiendo imagen de disco..."
qemu-img resize "$IMG_FILE" +2G

# ---------- 4. Habilitar SSH en la imagen ----------
echo ""
echo "[4/5] Habilitando SSH..."
# Montar partición boot y crear archivo 'ssh'
LOOP=$(sudo losetup -f --show -P "$IMG_FILE")
sudo mkdir -p /mnt/rpi-boot
sudo mount "${LOOP}p1" /mnt/rpi-boot
sudo touch /mnt/rpi-boot/ssh
sudo umount /mnt/rpi-boot
sudo losetup -d "$LOOP"
echo "      SSH habilitado."

# ---------- 5. Crear script de arranque ----------
echo ""
echo "[5/5] Creando script de arranque launch_qemu.sh..."

cat > launch_qemu.sh << 'EOF'
#!/bin/bash
# Lanza QEMU emulando Raspberry Pi
# SSH disponible en localhost:2222

WORKDIR="$(dirname "$0")"
cd "$WORKDIR"

qemu-system-arm \
  -kernel kernel-qemu \
  -cpu arm1176 \
  -m 256 \
  -M versatilepb \
  -dtb versatile-pb.dtb \
  -no-reboot \
  -serial stdio \
  -append "root=/dev/sda2 panic=1 rootfstype=ext4 rw" \
  -hda 2021-05-07-raspios-buster-armhf-lite.img \
  -net nic \
  -net user,hostfwd=tcp::2222-:22,hostfwd=tcp::8080-:8080 \
  -display none &

QEMU_PID=$!
echo "QEMU iniciado (PID=$QEMU_PID)"
echo ""
echo "Esperando que el sistema arranque (~30-60 segundos)..."
sleep 45

echo ""
echo "Intentando conexión SSH..."
echo "  Usuario: pi | Contraseña: raspberry"
echo ""
ssh -p 2222 -o StrictHostKeyChecking=no pi@localhost
EOF

chmod +x launch_qemu.sh

echo ""
echo "=============================================="
echo "  Setup completo."
echo ""
echo "  Para arrancar QEMU:"
echo "    cd $WORKDIR"
echo "    ./launch_qemu.sh"
echo ""
echo "  Credenciales por defecto:"
echo "    usuario   : pi"
echo "    contraseña: raspberry"
echo ""
echo "  Puertos expuestos al host:"
echo "    SSH  → localhost:2222"
echo "    HTTP → localhost:8080"
echo "=============================================="
