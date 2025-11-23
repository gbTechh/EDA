CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O0
TARGET = programa
PYTHON = python3
WORDCLOUD_SCRIPT = wordcloud_realtime.py

# Directorios
BUILD_DIR = output
SRC_DIR = .
JSON_OUTPUT = $(BUILD_DIR)/wordcloud_data.json

SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lista.cpp
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Colores para output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# ========== CREACIÓN DE DIRECTORIOS ==========

# Crear directorio de salida si no existe
$(shell mkdir -p $(BUILD_DIR))

# ========== REGLAS PRINCIPALES ==========

# Compilar el programa
$(TARGET): $(OBJECTS)
	@echo "$(GREEN)🔨 Enlazando $(TARGET)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJECTS)
	@echo "$(GREEN)✅ Compilación exitosa!$(NC)"

# Compilar archivos .cpp a .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "$(BLUE)📦 Compilando $<...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ========== REGLAS DE EJECUCIÓN ==========

# Ejecutar solo C++
run: $(TARGET)
	@echo "$(YELLOW)▶️  Ejecutando $(TARGET)...$(NC)"
	@cd $(BUILD_DIR) && ./$(TARGET)

# Ejecutar C++ con WordCloud en tiempo real
run-wordcloud: $(TARGET) check-python
	@echo "$(YELLOW)🚀 Iniciando sistema completo (C++ + Python WordCloud)...$(NC)"
	@echo "$(BLUE)Presiona Ctrl+C para detener$(NC)"
	@./run.sh

# Ejecutar solo el visualizador de Python
wordcloud: check-python
	@echo "$(YELLOW)📊 Iniciando visualizador de WordCloud...$(NC)"
	@if [ ! -f $(JSON_OUTPUT) ]; then \
		echo "$(RED)❌ Error: $(JSON_OUTPUT) no existe$(NC)"; \
		echo "$(YELLOW)Ejecuta primero 'make run' para generar datos$(NC)"; \
		exit 1; \
	fi
	$(PYTHON) $(WORDCLOUD_SCRIPT)

# ========== VERIFICACIÓN DE DEPENDENCIAS ==========

# Verificar que Python y las librerías estén instaladas
check-python:
	@echo "$(BLUE)🔍 Verificando dependencias de Python...$(NC)"
	@command -v $(PYTHON) >/dev/null 2>&1 || { \
		echo "$(RED)❌ Python3 no está instalado$(NC)"; \
		echo "$(YELLOW)Instala con: sudo pacman -S python$(NC)"; \
		exit 1; \
	}
	@$(PYTHON) -c "import matplotlib, wordcloud" 2>/dev/null || { \
		echo "$(RED)❌ Faltan librerías de Python$(NC)"; \
		echo "$(YELLOW)Ejecuta: make install-python-deps$(NC)"; \
		exit 1; \
	}
	@echo "$(GREEN)✅ Dependencias de Python verificadas$(NC)"

# Instalar dependencias de Python
install-python-deps:
	@echo "$(YELLOW)📦 Instalando dependencias de Python...$(NC)"
	pip install --user matplotlib wordcloud numpy pillow
	@echo "$(GREEN)✅ Dependencias instaladas$(NC)"

# ========== LIMPIEZA ==========

# Limpiar archivos compilados
clean:
	@echo "$(YELLOW)🧹 Limpiando archivos compilados...$(NC)"
	rm -rf $(BUILD_DIR)
	@echo "$(GREEN)✅ Limpieza completada$(NC)"

# Limpiar todo (incluye JSON generado)
clean-all: clean
	@echo "$(YELLOW)🧹 Limpiando todos los archivos generados...$(NC)"
	rm -f $(JSON_OUTPUT)
	@echo "$(GREEN)✅ Limpieza total completada$(NC)"

# ========== INSPECCIÓN ==========

# Mostrar estructura de archivos
list:
	@echo "$(BLUE)📁 Estructura del proyecto:$(NC)"
	@echo "$(YELLOW)Directorio principal:$(NC)"
	@ls -la
	@echo ""
	@echo "$(YELLOW)Directorio output/:$(NC)"
	@ls -la $(BUILD_DIR)/ 2>/dev/null || echo "No existe"

# ========== AYUDA ==========

# Mostrar ayuda
help:
	@echo "$(BLUE)════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  📋 Comandos disponibles del Makefile$(NC)"
	@echo "$(BLUE)════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Compilación:$(NC)"
	@echo "  make              - Compilar el proyecto en output/"
	@echo "  make clean        - Limpiar archivos compilados"
	@echo "  make clean-all    - Limpiar todo (incluye JSON)"
	@echo ""
	@echo "$(YELLOW)Ejecución:$(NC)"
	@echo "  make run          - Ejecutar desde output/"
	@echo "  make run-wordcloud- Ejecutar C++ + Python WordCloud"
	@echo "  make wordcloud    - Ver WordCloud de datos existentes"
	@echo ""
	@echo "$(YELLOW)Configuración:$(NC)"
	@echo "  make check-python - Verificar dependencias Python"
	@echo "  make install-python-deps - Instalar librerías Python"
	@echo "  make list         - Mostrar estructura de archivos"
	@echo ""
	@echo "$(YELLOW)Archivos generados en:$(NC)"
	@echo "  $(BUILD_DIR)/programa      - Ejecutable"
	@echo "  $(BUILD_DIR)/*.o          - Objetos"
	@echo "  $(BUILD_DIR)/wordcloud_data.json - Datos"
	@echo ""
	@echo "$(YELLOW)Otros:$(NC)"
	@echo "  make help         - Mostrar esta ayuda"
	@echo ""

# ========== CONFIGURACIÓN ==========

.PHONY: clean clean-all run run-wordcloud wordcloud check-python install-python-deps help list
.DEFAULT_GOAL := help