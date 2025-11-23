# Analizador de Trending Topics en Tiempo Real

## 📋 Prerrequisitos

- **Python 3.7+**
- **Compilador C++** (g++/clang++ en Linux)
- **pip** (gestor de paquetes de Python)

### 🐧 Linux (Arch/Ubuntu/Debian)

```bash
# En Arch Linux:
sudo pacman -S base-devel python python-pip

# En Ubuntu/Debian:
sudo apt update && sudo apt install build-essential python3 python3-pip python3-venv
```

## 🚀 Instalación Rápida

### 1. Crear y Activar Entorno Virtual

```bash
python -m venv venv
source venv/bin/activate
```

### 2. Instalar Dependencias de Python

```bash
pip install matplotlib wordcloud numpy pillow
```

### 3. Compilar Proyecto C++

```bash
make clean && make
```

### 4. Ejecutar con Visualización en Tiempo Real

```bash
make run-wordcloud
```

## 🛠️ Comandos Disponibles

### Compilación

```bash
make                    # Compilar solamente
make clean              # Limpiar archivos de compilación
```

### Ejecución

```bash
make run                # Ejecutar solo C++
make run-wordcloud      # Ejecutar C++ con word cloud en tiempo real
make wordcloud          # Ejecutar solo el visualizador de word cloud
```

### Utilidades

```bash
make install-python-deps    # Instalar dependencias de Python
make check-python           # Verificar dependencias Python
make list                   # Mostrar estructura de archivos
make help                   # Mostrar ayuda completa
```

## 🐛 Solución de Problemas

### Problemas Comunes

**Python no encontrado:**

```bash
python --version
# Si no funciona:
sudo pacman -S python python-pip
```

**Errores de compilación:**

```bash
# Verificar compilador
g++ --version
# Si no funciona:
sudo pacman -S base-devel
```

**Dependencias Python faltantes:**

```bash
make install-python-deps
```

**Word cloud no se muestra:**

- El script usa animación automática de matplotlib
- Verifica que tengas un backend gráfico funcionando

## Instalación Paso a Paso

### 1. Configurar entorno Python

```bash
python -m venv venv
source venv/bin/activate
pip install matplotlib wordcloud numpy pillow
```

### 2. Verificar instalación

```bash
python -c "import matplotlib, wordcloud; print('✅ Entorno configurado correctamente')"
```

### 3. Compilar proyecto C++

```bash
make clean && make
```

### 4. Ejecutar (opciones)

```bash
# C++ con visualización en tiempo real (RECOMENDADO)
make run-wordcloud

```

## 📝 Notas Importantes

- El proyecto genera archivos en la carpeta `output/`
- El word cloud se actualiza automáticamente cada 100 ms (cambiar la variable TIMESLEEP_MS en main.cpps)
- Para detener la ejecución: `Ctrl+C` en la terminal
- Los datos se guardan en `output/wordcloud_data.json`
- Asegurarse de tener archivos de texto en la carpeta `docs/` para procesar

## 🆘 Soporte

Si encuentras problemas:

1. Verificar dependencias: `make check-python`
2. Limpiar y recompilar: `make clean && make`
3. Verificar que exista la carpeta `docs/` con archivos de texto
4. Ejecutar `make list` para ver la estructura de archivos

---
