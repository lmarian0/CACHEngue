# CACHEngue
Proyectos de Sistemas De computacion

## md_to_pdf.sh

Script para convertir archivos Markdown a PDF usando pandoc.

### Cómo usarlo

```bash
./md_to_pdf.sh "ruta/relativa/del/archivo.md"
```

El PDF se generará en la misma carpeta que el markdown, con el mismo nombre.

**Ejemplos:**
```bash
./md_to_pdf.sh "TP1/TP1.md"                      # genera TP1/TP1.pdf
./md_to_pdf.sh "TP2/CAPA_2/README.md"            # genera TP2/CAPA_2/README.pdf
./md_to_pdf.sh "TP3_a/README.md"                 # genera TP3_a/README.pdf
```

### Requisitos

- pandoc
- texlive-xetex
- texlive-fonts-recommended

Para instalar (en Debian/Ubuntu):
```bash
sudo apt install pandoc texlive-xetex texlive-fonts-recommended
```

---

## Dependencias e Instalación

Instala todas las herramientas y dependencias necesarias para los TPs:

```bash
# Actualizar repositorios
sudo apt update

# Herramientas de compilación y desarrollo
sudo apt install -y build-essential gcc g++ make gdb

# Python y pip
sudo apt install -y python3 python3-pip

# Pandoc y LaTeX (para convertir MD a PDF)
sudo apt install -y pandoc texlive-xetex texlive-fonts-recommended

# Herramientas adicionales útiles
sudo apt install -y git vim nano curl wget
```

### Instalación individual por herramienta

Si prefieres instalar solo lo que necesites:

**Compilación en C/C++:**
```bash
sudo apt install -y build-essential gcc g++ make
```

**Debugging con GDB:**
```bash
sudo apt install -y gdb
```

**Python:**
```bash
sudo apt install -y python3 python3-pip
```

**Profiling con gprof:**
```bash
# Ya incluido en build-essential
```

**Conversión MD a PDF:**
```bash
sudo apt install -y pandoc texlive-xetex texlive-fonts-recommended
```
