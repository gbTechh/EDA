#!/usr/bin/env python3
import json
import time
import os
from wordcloud import WordCloud
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# Configuración
JSON_FILE = "wordcloud_data.json"
UPDATE_INTERVAL = 1

fig, ax = plt.subplots(figsize=(14, 9))
plt.subplots_adjust(left=0.05, right=0.95, top=0.92, bottom=0.08)

last_export_num = -1
inicio_tiempo = time.time()

def load_data():
    """Carga datos del JSON generado por C++"""
    try:
        if not os.path.exists(JSON_FILE):
            return None
        
        with open(JSON_FILE, 'r') as f:
            content = f.read()
            if not content.strip():
                return None
            data = json.loads(content)
        
        return data
    except (json.JSONDecodeError, FileNotFoundError, ValueError):
        return None

def update_wordcloud(frame):
    """Actualiza el wordcloud con nuevos datos"""
    global last_export_num
    
    data = load_data()
    
    if data is None:
        ax.clear()
        ax.text(0.5, 0.5, '⏳ Esperando datos de C++...\n\n(El programa está iniciando)', 
                horizontalalignment='center',
                verticalalignment='center',
                transform=ax.transAxes,
                fontsize=22,
                color='gray',
                style='italic')
        ax.axis('off')
        return
    
    current_export = data.get('export_num', 0)
    if current_export == last_export_num:
        return
    
    last_export_num = current_export
    
    topics = data.get('topics', [])
    
    if not topics or len(topics) == 0:
        ax.clear()
        ax.text(0.5, 0.5, '📝 Procesando tokens iniciales...\n\n⏱️ Espere un momento', 
                horizontalalignment='center',
                verticalalignment='center',
                transform=ax.transAxes,
                fontsize=22,
                color='orange',
                weight='bold')
        ax.axis('off')
        return
    
    # diccionario de frecuencias
    word_freq = {item['word']: item['frequency'] for item in topics}
    
    wc = WordCloud(
        width=1400, 
        height=900,
        background_color='white',
        colormap='plasma', 
        relative_scaling=0.4,
        min_font_size=12,
        max_words=100,
        prefer_horizontal=0.7,
        contour_width=2,
        contour_color='steelblue'
    ).generate_from_frequencies(word_freq)
    
    # Mostrar
    ax.clear()
    ax.imshow(wc, interpolation='bilinear')
    ax.axis('off')
    
    ventana = data.get('ventana', 0)
    tokens = data.get('timestamp', 0)
    tiempo_transcurrido = time.time() - inicio_tiempo
    
    titulo = f'Top {len(topics)} Topics en Tiempo Real | Ventana: {ventana} | Tokens: {tokens} | Actualización #{current_export}'
    ax.set_title(titulo, fontsize=14, fontweight='bold', pad=15, color='darkblue')
    
    top5_text = "Top 5: " + " | ".join([f"{item['word']} ({item['frequency']})" for item in topics[:5]])
    fig.text(0.5, 0.02, top5_text, ha='center', fontsize=11, color='darkgreen', weight='bold')
    
    fig.text(0.5, 0.96, f'⏱Tiempo: {tiempo_transcurrido:.1f}s | 🚀 Tokens/seg: {tokens/tiempo_transcurrido:.1f}', 
             ha='center', fontsize=10, color='gray', style='italic')
    
    print(f"\rUpdate #{current_export:4d} | Tokens: {tokens:6d} | Top: ", end='')
    for i, item in enumerate(topics[:3]):
        print(f"{item['word']}({item['frequency']})", end=' ')
    print("     ", end='', flush=True)

plt.get_current_fig_manager().set_window_title('WordCloud - Análisis en Tiempo Real')

ani = FuncAnimation(fig, update_wordcloud, interval=UPDATE_INTERVAL, cache_frame_data=False)


print(f"Archivo: {JSON_FILE}")
print(f"Frecuencia: cada {UPDATE_INTERVAL}ms")
print(f"Mantén esta ventana abierta para ver actualizaciones")

try:
    plt.show()
except KeyboardInterrupt:
    print("\n\n✅ WordCloud cerrado por el usuario")