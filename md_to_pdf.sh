#!/bin/bash
# md_to_pdf.sh
# Convierte un archivo .md a PDF usando pandoc.
# Las imagenes referenciadas en el markdown se resuelven
# relativas a la carpeta donde esta el .md, no a donde
# se ejecuta el script.
#
# Uso:
#   ./md_to_pdf.sh "TP3_a/README.md"
#
# El PDF se guarda en la misma carpeta que el .md:
#   TP3_a/README.pdf
#
# Requiere: pandoc y xelatex
#   sudo apt install pandoc texlive-xetex texlive-fonts-recommended

if [ -z "$1" ]; then
  echo "Uso: ./md_to_pdf.sh \"ruta/al/archivo.md\""
  echo "Ejemplo: ./md_to_pdf.sh \"TP3_a/README.md\""
  exit 1
fi

INPUT="$1"

if [ ! -f "$INPUT" ]; then
  echo "Error: no se encontro el archivo '$INPUT'"
  exit 1
fi

# Carpeta donde vive el .md (para resolver imagenes relativas)
MD_DIR="$(dirname "$INPUT")"

# Nombre del PDF: mismo nombre que el .md, misma carpeta
BASENAME="$(basename "$INPUT" .md)"
OUTPUT="$MD_DIR/$BASENAME.pdf"

echo "Convirtiendo: $INPUT"
echo "Guardando en: $OUTPUT"

pandoc "$INPUT" \
  -o "$OUTPUT" \
  --pdf-engine=xelatex \
  --resource-path="$MD_DIR" \
  -V geometry:margin=2.5cm \
  -V fontsize=11pt \
  -V linkcolor=blue \
  --highlight-style=tango

if [ $? -eq 0 ]; then
  echo "Listo! PDF generado: $OUTPUT"
else
  echo ""
  echo "Error al generar el PDF."
  echo "Si falta latex, instala con:"
  echo "  sudo apt install texlive-xetex texlive-fonts-recommended"
fi