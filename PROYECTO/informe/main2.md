---
title: "Sistema de Detección de Trending Topics usando Space-Saving y Ventanas Deslizantes"
author: "Rodrigo Silva"
date: "2025"
geometry: "margin=2.5cm"
header-includes:
  - \usepackage{./header.tex}
---

\begin{abstract}
Este artículo presenta un sistema eficiente para la detección de trending topics en tiempo real utilizando la estructura de datos \textit{Space-Saving} combinada con ventanas deslizantes. El enfoque permite identificar temas emergentes dentro de flujos de texto extensos, manteniendo uso constante de memoria y generando \textbf{wordclouds dinámicos} que visualizan la evolución de las tendencias por ventana temporal.
\end{abstract}

\textbf{Palabras clave:} Space-Saving, Trending Topics, Wordcloud, Ventanas Deslizantes, Procesamiento de Texto

\section{Introducción}

La detección de tendencias en grandes volúmenes de texto, como publicaciones o noticias, requiere sistemas que identifiquen cambios temáticos de manera continua y eficiente. Procesar cada documento desde cero es inviable, por lo que se emplean algoritmos de \textit{stream mining} que operan con memoria constante. En este trabajo se implementa una arquitectura basada en la estructura \textbf{Space-Saving} y el uso de \textbf{ventanas deslizantes} de tamaño $k$, para observar cómo cambian los temas predominantes a medida que nuevos textos son procesados.

\section{Arquitectura General del Sistema}

\begin{longtable}{|p{4cm}|p{5.5cm}|p{6.5cm}|}
\hline
\textbf{Componente} & \textbf{Función} & \textbf{Ventaja} \\
\hline
Preprocesador & Limpieza, minúsculas, eliminación de stopwords y tokenización & Estandariza los datos antes del conteo \\
\hline
Estructura Space-Saving & Conteo eficiente de frecuencias con memoria constante & O(log K) por operación \\
\hline
Ventana Deslizante & Procesamiento de los últimos $k$ textos & Captura la evolución de temas en el tiempo \\
\hline
Wordcloud Dinámico & Visualización de los Top-K tokens por ventana & Muestra la evolución temática \\
\hline
\end{longtable}

\subsection{Flujo del Proceso}
\begin{verbatim}
TXT → Preprocesamiento → Space-Saving → Top-K → Wordcloud
\end{verbatim}

\textbf{Características:}
\begin{itemize}
\item Procesamiento incremental por lotes de tamaño $k$
\item Eliminación de textos antiguos al deslizar la ventana
\item Memoria fija e independiente del total de palabras
\item Wordcloud generado por cada ventana para visualizar tendencias
\end{itemize}

\section{Metodología}

\subsection{Preprocesamiento de Texto}

\textbf{Ejemplo:}
\begin{verbatim}
"El partido de fútbol estuvo INCREÍBLE! #Deporte"
\end{verbatim}

\textbf{Transformaciones:}
\begin{enumerate}
\item Minúsculas → "el partido de fútbol estuvo increíble \#deporte"
\item Limpieza → "partido fútbol estuvo increíble deporte"
\item Eliminación de stopwords → "partido fútbol increíble deporte"
\item Lematizacion
\item Tokenización → ["partido", "fútbol", "increíble", "deporte"]
\end{enumerate}

\subsection{Algoritmo Space-Saving}

Cada token es procesado para mantener solo los $K$ más frecuentes:
\begin{itemize}
\item Si el token ya está en el contador, su frecuencia aumenta.
\item Si hay espacio libre, se añade con frecuencia 1.
\item Si el contador está lleno, se reemplaza el token con menor frecuencia.
\end{itemize}

\textbf{Ejemplo con $K=3$:}
\begin{verbatim}
[("fútbol", 4), ("gol", 2), ("partido", 1)]
\end{verbatim}
Al llegar una nueva palabra ("terremoto", 3), reemplaza al menor si su frecuencia es superior.

\section{Procesamiento con Ventanas Deslizantes}

El sistema procesa los textos por grupos consecutivos de tamaño $k$.  
Por ejemplo, si $k=3$ y existen 8 textos totales:

\textbf{Ventana 1: [1–3]}  
Top-K: ["fútbol", "gol", "partido"]  
Wordcloud: \textbf{FÚTBOL} gol partido

\textbf{Ventana 2: [2–4]}  
Top-K: ["terremoto", "fútbol", "gol"]  
Wordcloud: \textbf{TERREMOTO} fútbol gol

\textbf{Ventana 3: [3–5]}  
Top-K: ["terremoto", "alerta", "ciencia"]  
Wordcloud: \textbf{TERREMOTO} alerta ciencia

\textbf{Evolución temática:}
\begin{verbatim}
Ventana 1: [fútbol, gol, partido]
Ventana 2: [terremoto, fútbol, gol]
Ventana 3: [terremoto, alerta, ciencia]
\end{verbatim}

\section{Resultados y Análisis}

El sistema demuestra:
\begin{itemize}
\item \textbf{Reacción inmediata} ante aparición de nuevos temas.
\item \textbf{Memoria constante} gracias al algoritmo Space-Saving.
\item \textbf{Actualización eficiente} por ventana deslizante sin reprocesamiento total.
\item \textbf{Wordclouds evolutivos} que muestran el cambio de tendencias a lo largo del flujo.
\end{itemize}

\section{Conclusión y Trabajo Futuro}

El modelo propuesto ofrece una alternativa \textbf{ligera y escalable} para detección de tendencias en flujos de texto extensos.  
Su simplicidad lo hace ideal para aplicaciones académicas o prototipos en tiempo real.

\textbf{Futuras mejoras:}
\begin{itemize}
\item Incorporar detección de picos usando \textbf{EWMA} para medir cambios abruptos.
\item Experimentar con tamaños de ventana variables.
\item Integrar visualizaciones en tiempo real y métricas de estabilidad.
\end{itemize}
