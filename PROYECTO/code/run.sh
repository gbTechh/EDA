#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROGRAMA="./output/programa"
PYTHON_SCRIPT="wordcloud_realtime.py"
JSON_FILE="wordcloud_data.json"
PYTHON_PID=""

cleanup() {
    echo -e "\n${YELLOW}🛑 Deteniendo procesos...${NC}"
    if [ ! -z "$PYTHON_PID" ]; then
        kill $PYTHON_PID 2>/dev/null
        echo -e "${GREEN}✅ Python detenido${NC}"
    fi
    pkill -f "$PYTHON_SCRIPT" 2>/dev/null
    echo -e "${GREEN}✅ Limpieza completada${NC}"
    exit 0
}

trap cleanup SIGINT SIGTERM

if [ ! -f "$PROGRAMA" ]; then
    echo -e "${RED}❌ Error: $PROGRAMA no existe${NC}"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo -e "${RED}❌ Python3 no instalado${NC}"
    exit 1
fi

python3 -c "import matplotlib, wordcloud" 2>/dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Faltan librerías Python${NC}"
    exit 1
fi

if [ ! -f "$PYTHON_SCRIPT" ]; then
    echo -e "${RED}❌ $PYTHON_SCRIPT no existe${NC}"
    exit 1
fi

echo -e "${BLUE}════════════════════════════════════════════════${NC}"
echo -e "${GREEN}  🚀 Sistema de WordCloud en Tiempo Real${NC}"
echo -e "${BLUE}════════════════════════════════════════════════${NC}"
echo ""

[ -f "$JSON_FILE" ] && rm -f "$JSON_FILE"

echo -e "${BLUE}🐍 Iniciando Python...${NC}"
python3 "$PYTHON_SCRIPT" &
PYTHON_PID=$!

sleep 2

if ! ps -p $PYTHON_PID > /dev/null; then
    echo -e "${RED}❌ Python falló${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Python iniciado (PID: $PYTHON_PID)${NC}"
echo ""
echo -e "${BLUE}▶️  Ejecutando C++...${NC}"
echo -e "${YELLOW}El WordCloud seguirá visible después de que C++ termine${NC}"
echo -e "${YELLOW}Presiona Ctrl+C para cerrar todo${NC}"
echo ""

# Ejecutar C++ en primer plano
$PROGRAMA

echo ""
echo -e "${GREEN}✅ Programa C++ finalizado${NC}"
echo -e "${YELLOW}⏸️  WordCloud sigue activo - Presiona Ctrl+C para cerrar${NC}"

# ⭐ ESPERAR indefinidamente - Python sigue mostrando el WordCloud
wait $PYTHON_PID