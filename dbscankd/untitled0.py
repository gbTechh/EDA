# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D
from sklearn.decomposition import PCA
from sklearn.metrics import silhouette_score
import seaborn as sns
import glob
import os

# Crear carpeta img si no existe
if not os.path.exists('img'):
    os.makedirs('img')

def encontrar_archivos_clustering(carpeta_base="."):
    """
    Encuentra automáticamente todos los archivos de resultados de clustering
    en las carpetas resultados_dbscan y resultados_dbscan_kdtree
    """
    patrones = [
        "./data/resultados_dbscan/*_dbscan.csv",
        "./data/resultados_dbscan_kdtree/*_dbscan.csv",
    ]
    
    archivos = []
    for patron in patrones:
        archivos.extend(glob.glob(patron))
    
    print("Archivos de clustering encontrados:")
    for archivo in archivos:
        print(f"  - {archivo}")
    
    return archivos

def visualizar_clusters_multiples():
    """
    Visualiza clusters de todos los archivos encontrados
    """
    archivos = encontrar_archivos_clustering()

    if not archivos:
        print("No se encontraron archivos de clustering")
        return

    metricas_totales = []

    for archivo in archivos:
        print(f"\n{'='*60}")
        print(f"PROCESANDO: {archivo}")
        print(f"{'='*60}")

        try:
            # Leer los datos
            df = pd.read_csv(archivo)
            
            # Verificar si la primera fila es el nombre del archivo
            if 'archivo' in df.columns and df.shape[1] == 1:
                # Saltar la primera fila que contiene solo el nombre del archivo
                df = pd.read_csv(archivo, skiprows=1)

            # Extraer características (dimensiones)
            dimension_cols = [col for col in df.columns if col.startswith('Dimension_')]
            if not dimension_cols:
                print(f"  ❌ No se encontraron columnas de dimensiones en {archivo}")
                continue

            # Extraer clusters
            if 'Cluster' not in df.columns:
                print(f"  ❌ No se encontró columna 'Cluster' en {archivo}")
                continue

            X = df[dimension_cols].values
            clusters = df['Cluster'].values

            n_dimensiones = len(dimension_cols)
            n_clusters = len(np.unique(clusters[clusters != -1]))
            outlier_count = np.sum(clusters == -1)

            print(f"  Dimensiones: {n_dimensiones}D")
            print(f"  Puntos: {len(X)}")
            print(f"  Clusters: {n_clusters}")
            print(f"  Outliers: {outlier_count}")

            # Calcular métricas
            if n_clusters > 1 and len(X[clusters != -1]) > n_clusters:
                mask = clusters != -1
                if len(np.unique(clusters[mask])) > 1:  # Asegurar que hay al menos 2 clusters
                    silhouette_avg = silhouette_score(X[mask], clusters[mask])
                    print(f"  Silhouette Score: {silhouette_avg:.4f}")
                else:
                    silhouette_avg = None
                    print(f"  Silhouette Score: No calculable (solo 1 cluster después de filtrar outliers)")
            else:
                silhouette_avg = None
                print(f"  Silhouette Score: No calculable")

            # Crear visualización
            fig = plt.figure(figsize=(18, 12))
            
            # Extraer nombre del dataset del path del archivo
            dataset_name = os.path.basename(archivo).replace('_dbscan.csv', '')
            folder_name = os.path.basename(os.path.dirname(archivo))
            
            fig.suptitle(f'Dataset: {dataset_name} | Método: {folder_name}\n({n_dimensiones}D) - Puntos: {len(X)}, Clusters: {n_clusters}',
                        fontsize=16, fontweight='bold')

            # Visualización 2D con PCA - CORREGIDO
            ax1 = plt.subplot(2, 3, 1)
            pca = PCA(n_components=2)
            X_reduced = pca.fit_transform(X)

            # CORRECCIÓN: Usar valores numéricos consistentes para colores
            plot_colors = np.where(clusters == -1, -1, clusters)
            scatter = ax1.scatter(X_reduced[:, 0], X_reduced[:, 1], c=plot_colors, 
                                cmap='tab10', alpha=0.7, s=50)
            
            # Añadir leyenda para outliers
            if outlier_count > 0:
                outlier_mask = clusters == -1
                ax1.scatter(X_reduced[outlier_mask, 0], X_reduced[outlier_mask, 1], 
                           c='red', alpha=0.7, s=50, label='Outliers')
                ax1.legend()
            
            title_silhouette = f'PCA 2D\nSilhouette: {silhouette_avg:.4f}' if silhouette_avg else 'PCA 2D'
            ax1.set_title(title_silhouette, fontweight='bold')
            ax1.set_xlabel(f'PC1 ({pca.explained_variance_ratio_[0]:.2%} var.)')
            ax1.set_ylabel(f'PC2 ({pca.explained_variance_ratio_[1]:.2%} var.)')

            # Distribución de clusters
            ax2 = plt.subplot(2, 3, 2)
            cluster_counts = pd.Series(clusters).value_counts().sort_index()
            colors_bar = ['red' if idx == -1 else f'C{idx % 10}' for idx in cluster_counts.index]
            bars = ax2.bar(range(len(cluster_counts)), cluster_counts.values, color=colors_bar)
            ax2.set_title('Distribución de Clusters', fontweight='bold')
            ax2.set_xlabel('Cluster ID')
            ax2.set_ylabel('Número de Puntos')
            ax2.set_xticks(range(len(cluster_counts)))
            ax2.set_xticklabels([f'{"Ruido" if idx == -1 else idx}' for idx in cluster_counts.index])
            
            # Añadir números en las barras
            for bar, count in zip(bars, cluster_counts.values):
                ax2.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.1, 
                        str(count), ha='center', va='bottom')

            # Proyección 3D - CORREGIDO
            ax3 = plt.subplot(2, 3, 3, projection='3d')
            if n_dimensiones >= 3:
                pca_3d = PCA(n_components=3)
                X_3d = pca_3d.fit_transform(X)
                # CORRECCIÓN: Usar valores numéricos consistentes
                scatter_3d = ax3.scatter(X_3d[:, 0], X_3d[:, 1], X_3d[:, 2],
                                       c=plot_colors, cmap='tab10', alpha=0.7, s=30)
                ax3.set_title(f'PCA 3D\n({pca_3d.explained_variance_ratio_.sum():.2%} var. explicada)', 
                            fontweight='bold')
                ax3.set_xlabel('PC1')
                ax3.set_ylabel('PC2')
                ax3.set_zlabel('PC3')
            else:
                # Para 2D, mostrar scatter plot de las primeras 2 dimensiones
                scatter_3d = ax3.scatter(X[:, 0], X[:, 1], c=plot_colors, cmap='tab10', alpha=0.7, s=30)
                ax3.set_title('Dimensiones Originales 1-2', fontweight='bold')
                ax3.set_xlabel('Dimensión 1')
                ax3.set_ylabel('Dimensión 2')

            # Resto del código permanece igual...
            # Heatmap de correlación entre dimensiones
            ax4 = plt.subplot(2, 3, 4)
            corr_matrix = df[dimension_cols].corr()
            sns.heatmap(corr_matrix, annot=True, cmap='coolwarm', center=0, ax=ax4)
            ax4.set_title('Correlación entre Dimensiones', fontweight='bold')

            # Distribución de outliers vs inliers
            ax5 = plt.subplot(2, 3, 5)
            outlier_labels = ['Inliers', 'Outliers']
            outlier_counts = [len(X) - outlier_count, outlier_count]
            colors_pie = ['lightblue', 'red']
            ax5.pie(outlier_counts, labels=outlier_labels, autopct='%1.1f%%', 
                   colors=colors_pie, startangle=90)
            ax5.set_title('Proporción Outliers/Inliers', fontweight='bold')

            # Información del dataset
            ax6 = plt.subplot(2, 3, 6)
            ax6.axis('off')
            
            silhouette_text = f"{silhouette_avg:.4f}" if silhouette_avg is not None else "N/A"
            metodo = folder_name.replace('resultados_dbscan', '').replace('_', ' ').strip()
            metodo = 'Búsqueda Lineal' if metodo == '' else 'KD-Tree'

            info_text = f"""INFORMACIÓN DEL DATASET:
            
Dataset: {dataset_name}
Método: {metodo}
Dimensiones: {n_dimensiones}D
Puntos totales: {len(X)}
Clusters encontrados: {n_clusters}
Puntos por cluster: {len(X) - outlier_count}
Outliers: {outlier_count}
Silhouette Score: {silhouette_text}

ARCHIVO: {archivo}"""

            ax6.text(0.1, 0.9, info_text, fontsize=11, fontfamily='monospace',
                    verticalalignment='top', transform=ax6.transAxes,
                    bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray"))

            plt.tight_layout()
            
            # Guardar la figura en carpeta img
            output_filename = f"img/clustering_{dataset_name}_{metodo.replace(' ', '_')}.png"
            plt.savefig(output_filename, dpi=300, bbox_inches='tight')
            print(f"  Gráfico guardado como: {output_filename}")
            
            plt.close()  # Cerrar la figura para liberar memoria

            # Guardar métricas
            metricas_totales.append({
                'Dataset': dataset_name,
                'Metodo': metodo,
                'Dimensiones': n_dimensiones,
                'Puntos': len(X),
                'Clusters': n_clusters,
                'Outliers': outlier_count,
                'Silhouette': silhouette_avg,
                'Archivo': archivo
            })

        except Exception as e:
            print(f"  ❌ Error procesando {archivo}: {e}")
            import traceback
            traceback.print_exc()  # Esto te dará más detalles del error
            continue

    # Mostrar resumen final
    if metricas_totales:
        print(f"\n{'='*80}")
        print("RESUMEN FINAL DE TODOS LOS DATASETS")
        print(f"{'='*80}")

        df_metricas = pd.DataFrame(metricas_totales)
        print(df_metricas.to_string(index=False))

        # Guardar resumen en CSV
        df_metricas.to_csv('resumen_metricas_clustering.csv', index=False)
        print(f"\nResumen guardado en: resumen_metricas_clustering.csv")

    return metricas_totales

def encontrar_archivos_tiempos(carpeta_base="."):
    """
    Encuentra automáticamente todos los archivos de tiempos de ejecución
    """
    patrones = [
        "./data/resultados_dbscan/tiempo_ejecucion_*.csv",
        "./data/resultados_dbscan_kdtree/tiempo_ejecucion_*.csv", 
    ]
    
    archivos = []
    for patron in patrones:
        archivos.extend(glob.glob(patron))
    
    print("Archivos de tiempos encontrados:")
    for archivo in archivos:
        print(f"  - {archivo}")
    
    return archivos

def combinar_tiempos_multiples():
    """
    Combina todos los archivos de tiempos en un solo DataFrame
    """
    archivos = encontrar_archivos_tiempos()

    if not archivos:
        print("No se encontraron archivos de tiempos")
        return None

    todos_tiempos = []

    for archivo in archivos:
        try:
            df = pd.read_csv(archivo)
            # Extraer nombre del dataset y método del path
            dataset_name = os.path.basename(archivo).replace('tiempo_ejecucion_', '').replace('.csv', '')
            folder_name = os.path.basename(os.path.dirname(archivo))
            metodo = 'DBSCAN_KDTree' if 'kdtree' in folder_name.lower() else 'DBSCAN'
            
            df['Dataset'] = dataset_name
            df['Metodo'] = metodo
            df['Carpeta'] = folder_name
            todos_tiempos.append(df)
        except Exception as e:
            print(f"Error leyendo {archivo}: {e}")

    if todos_tiempos:
        df_combinado = pd.concat(todos_tiempos, ignore_index=True)
        return df_combinado
    else:
        return None

def crear_graficas_comparativas_tiempos(df):
    """
    Crea gráficas comparativas completas de tiempos de ejecución
    """
    if df is None or df.empty:
        print("No hay datos para crear gráficas comparativas")
        return
    
    print("\nCreando gráficas comparativas de tiempos...")
    
    # Configuración de estilo
    plt.style.use('default')
    sns.set_palette("husl")
    
    # ========== GRÁFICA 1: Comparación lado a lado ==========
    plt.figure(figsize=(14, 8))
    
    # Preparar datos
    pivot_df = df.pivot_table(index='Dataset', columns='Metodo', values='tiempo_ejecucion_ms', aggfunc='mean')
    
    # Gráfico de barras agrupadas
    x = np.arange(len(pivot_df.index))
    width = 0.35
    
    plt.bar(x - width/2, pivot_df['DBSCAN'], width, label='DBSCAN (Búsqueda Lineal)', alpha=0.8, color='skyblue')
    plt.bar(x + width/2, pivot_df['DBSCAN_KDTree'], width, label='DBSCAN (KD-Tree)', alpha=0.8, color='lightcoral')
    
    plt.xlabel('Dataset', fontsize=12, fontweight='bold')
    plt.ylabel('Tiempo de Ejecución (ms)', fontsize=12, fontweight='bold')
    plt.title('Comparación de Tiempos de Ejecución: DBSCAN vs DBSCAN-KDTree', 
              fontsize=14, fontweight='bold', pad=20)
    plt.xticks(x, pivot_df.index, rotation=45, ha='right')
    plt.legend()
    plt.grid(True, alpha=0.3, axis='y')
    
    # Añadir valores en las barras
    for i, v in enumerate(pivot_df['DBSCAN']):
        plt.text(i - width/2, v + 0.1, f'{v:.0f}ms', ha='center', va='bottom', fontsize=9)
    for i, v in enumerate(pivot_df['DBSCAN_KDTree']):
        plt.text(i + width/2, v + 0.1, f'{v:.0f}ms', ha='center', va='bottom', fontsize=9)
    
    plt.tight_layout()
    plt.savefig('img/comparacion_tiempos_barras.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # ========== GRÁFICA 2: Speedup KD-Tree ==========
    plt.figure(figsize=(12, 6))
    
    if 'DBSCAN' in pivot_df.columns and 'DBSCAN_KDTree' in pivot_df.columns:
        speedup = pivot_df['DBSCAN'] / pivot_df['DBSCAN_KDTree']
        
        colors = ['green' if x > 1 else 'red' for x in speedup]
        bars = plt.bar(speedup.index, speedup.values, color=colors, alpha=0.7)
        
        plt.axhline(y=1, color='black', linestyle='--', alpha=0.5, label='Límite neutral')
        plt.xlabel('Dataset', fontsize=12, fontweight='bold')
        plt.ylabel('Factor de Speedup', fontsize=12, fontweight='bold')
        plt.title('Speedup: KD-Tree vs Búsqueda Lineal\n(Valores > 1 indican que KD-Tree es más rápido)', 
                 fontsize=14, fontweight='bold', pad=20)
        plt.xticks(rotation=45, ha='right')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # Añadir valores en las barras
        for bar, value in zip(bars, speedup.values):
            plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.05, 
                    f'{value:.1f}x', ha='center', va='bottom', fontweight='bold')
        
        plt.tight_layout()
        plt.savefig('img/speedup_kdtree.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # ========== GRÁFICA 3: Líneas temporales ==========
    plt.figure(figsize=(14, 8))
    
    for metodo in df['Metodo'].unique():
        mask = df['Metodo'] == metodo
        datos_metodo = df[mask].sort_values('Dataset')
        plt.plot(datos_metodo['Dataset'], datos_metodo['tiempo_ejecucion_ms'], 
                marker='o', linewidth=2, markersize=8, label=metodo)
    
    plt.xlabel('Dataset', fontsize=12, fontweight='bold')
    plt.ylabel('Tiempo de Ejecución (ms)', fontsize=12, fontweight='bold')
    plt.title('Evolución de Tiempos de Ejecución por Dataset', 
              fontsize=14, fontweight='bold', pad=20)
    plt.xticks(rotation=45, ha='right')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('img/lineas_tiempos.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # ========== GRÁFICA 4: Heatmap de tiempos ==========
    plt.figure(figsize=(10, 6))
    
    heatmap_data = pivot_df.copy()
    sns.heatmap(heatmap_data, annot=True, fmt='.0f', cmap='YlOrRd', 
                cbar_kws={'label': 'Tiempo (ms)'}, linewidths=0.5)
    plt.title('Mapa de Calor: Tiempos de Ejecución por Dataset y Método', 
              fontsize=14, fontweight='bold', pad=20)
    plt.tight_layout()
    plt.savefig('img/heatmap_tiempos.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # ========== GRÁFICA 5: Boxplot comparativo ==========
    plt.figure(figsize=(10, 6))
    
    sns.boxplot(data=df, x='Metodo', y='tiempo_ejecucion_ms', palette=['skyblue', 'lightcoral'])
    plt.xlabel('Método', fontsize=12, fontweight='bold')
    plt.ylabel('Tiempo de Ejecución (ms)', fontsize=12, fontweight='bold')
    plt.title('Distribución de Tiempos de Ejecución por Método', 
              fontsize=14, fontweight='bold', pad=20)
    plt.grid(True, alpha=0.3, axis='y')
    
    # Añadir puntos individuales
    sns.stripplot(data=df, x='Metodo', y='tiempo_ejecucion_ms', 
                 color='black', alpha=0.6, size=5, jitter=True)
    
    plt.tight_layout()
    plt.savefig('img/boxplot_tiempos.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    # ========== GRÁFICA 6: Porcentaje de mejora ==========
    plt.figure(figsize=(12, 6))
    
    if 'DBSCAN' in pivot_df.columns and 'DBSCAN_KDTree' in pivot_df.columns:
        mejora_porcentual = ((pivot_df['DBSCAN'] - pivot_df['DBSCAN_KDTree']) / pivot_df['DBSCAN']) * 100
        
        colors = ['green' if x > 0 else 'red' for x in mejora_porcentual]
        bars = plt.bar(mejora_porcentual.index, mejora_porcentual.values, color=colors, alpha=0.7)
        
        plt.axhline(y=0, color='black', linestyle='-', alpha=0.3)
        plt.xlabel('Dataset', fontsize=12, fontweight='bold')
        plt.ylabel('Mejora Porcentual (%)', fontsize=12, fontweight='bold')
        plt.title('Mejora Porcentual: KD-Tree vs Búsqueda Lineal\n(Valores positivos indican mejora)', 
                 fontsize=14, fontweight='bold', pad=20)
        plt.xticks(rotation=45, ha='right')
        plt.grid(True, alpha=0.3)
        
        # Añadir valores en las barras
        for bar, value in zip(bars, mejora_porcentual.values):
            plt.text(bar.get_x() + bar.get_width()/2, 
                    bar.get_height() + (1 if value > 0 else -3), 
                    f'{value:.1f}%', ha='center', va='bottom' if value > 0 else 'top', 
                    fontweight='bold', fontsize=10)
        
        plt.tight_layout()
        plt.savefig('img/mejora_porcentual.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    # ========== GRÁFICA 7: Resumen estadístico ==========
    plt.figure(figsize=(12, 8))
    plt.axis('off')
    
    # Calcular estadísticas
    stats_text = "RESUMEN ESTADÍSTICO COMPARATIVO\n\n"
    
    for metodo in df['Metodo'].unique():
        mask = df['Metodo'] == metodo
        tiempos = df[mask]['tiempo_ejecucion_ms']
        
        stats_text += f"=== {metodo} ===\n"
        stats_text += f"Media: {tiempos.mean():.2f} ms\n"
        stats_text += f"Mediana: {tiempos.median():.2f} ms\n"
        stats_text += f"Mínimo: {tiempos.min():.2f} ms\n"
        stats_text += f"Máximo: {tiempos.max():.2f} ms\n"
        stats_text += f"Desviación estándar: {tiempos.std():.2f} ms\n"
        stats_text += f"Total: {tiempos.sum():.2f} ms\n\n"
    
    # Calcular mejora promedio
    if 'DBSCAN' in pivot_df.columns and 'DBSCAN_KDTree' in pivot_df.columns:
        mejora_promedio = ((pivot_df['DBSCAN'].mean() - pivot_df['DBSCAN_KDTree'].mean()) / pivot_df['DBSCAN'].mean()) * 100
        stats_text += f"MEJORA PROMEDIO CON KD-TREE: {mejora_promedio:.1f}%\n"
        stats_text += f"SPEEDUP PROMEDIO: {pivot_df['DBSCAN'].mean() / pivot_df['DBSCAN_KDTree'].mean():.2f}x"
    
    plt.text(0.1, 0.95, stats_text, fontsize=12, fontfamily='monospace',
            verticalalignment='top', transform=plt.gca().transAxes,
            bbox=dict(boxstyle="round,pad=0.5", facecolor="lightgray", alpha=0.8))
    
    plt.tight_layout()
    plt.savefig('img/resumen_estadistico.png', dpi=300, bbox_inches='tight')
    plt.close()
    
    print("✅ Todas las gráficas comparativas han sido guardadas en la carpeta 'img'")

def comparar_tiempos_multiples():
    """
    Compara tiempos de ejecución de todos los datasets
    """
    df = combinar_tiempos_multiples()

    if df is None or df.empty:
        print("No hay datos para comparar")
        return

    print("\nDatos de tiempos combinados:")
    print(df)

    # Crear gráficas comparativas
    crear_graficas_comparativas_tiempos(df)

    # Guardar datos combinados
    df.to_csv('tiempos_combinados.csv', index=False)
    print(f"\nDatos de tiempos combinados guardados en: tiempos_combinados.csv")

    return df

# Ejecutar análisis completo
if __name__ == "__main__":
    print("=== ANÁLISIS DE RESULTADOS DBSCAN ===")
    
    # 1. Visualizar clusters
    print("\n1. VISUALIZANDO CLUSTERS...")
    metricas = visualizar_clusters_multiples()
    
    # 2. Comparar tiempos
    print("\n2. COMPARANDO TIEMPOS DE EJECUCIÓN...")
    tiempos = comparar_tiempos_multiples()
    
    print("\n=== ANÁLISIS COMPLETADO ===")
    print("📁 Todas las imágenes han sido guardadas en la carpeta 'img'")
    print("📊 Resumen de métricas guardado en: resumen_metricas_clustering.csv")
    print("⏱️  Datos de tiempos guardados en: tiempos_combinados.csv")