#!/bin/bash

echo "🚀 Compilando Documento de Ejecución de ARKA..."

# Verificar que pandoc está instalado
if ! command -v pandoc &> /dev/null; then
    echo "❌ Error: Pandoc no está instalado"
    echo "Instálalo con: sudo apt install pandoc (Ubuntu) o sudo pacman -S pandoc (Arch)"
    exit 1
fi

# Crear directorio de salida si no existe
mkdir -p output

# Compilar el documento principal SIN TOC (índice) y CON bibliografía
pandoc main.md \
  --include-in-header=header.tex \
  --pdf-engine=pdflatex \
  --number-sections \
  --variable geometry:margin=2cm \
  --variable fontsize:08pt \
  --variable papersize:a4 \
  --variable colorlinks:true \
  --variable linkcolor:blue \
  --variable urlcolor:blue \
  --variable citecolor:blue \
  --variable inputenc:utf8 \
  --variable fontenc:T1 \
  -o output/informe.pdf

if [ $? -eq 0 ]; then
    echo "✅ Documento compilado exitosamente: output/informe.pdf"
else
    echo "❌ Error al compilar el documento"
    echo "Verifica que todos los archivos estén presentes:"
    echo "  - main.md"
    echo "  - referencias.bib (nuevo archivo)"
fi