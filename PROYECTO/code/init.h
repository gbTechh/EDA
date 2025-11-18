#ifndef INIT_H
#define INIT_H

#include "data.h"
#include "map.h"
#include "preprocesador_cpp.h"
#include "topic.h"
#include "vector.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

const std::string RESET = "\033[0m";
const std::string ROJO = "\033[31m";
const std::string VERDE = "\033[32m";
const std::string AMARILLO = "\033[33m";
const std::string AZUL = "\033[34m";

namespace fs = std::filesystem;

template <class T> struct Fd_String {
  inline unsigned long operator()(const T &x) const {
    unsigned long hash_value = 0;
    int i = 1;
    for (char c : x) {
      hash_value += static_cast<unsigned long>(c) * i + i;
      i++;
    }
    return hash_value;
  }
};

struct Init {
  int k;
  int bucket_size;
  int tokens_ventana;
  int documentos_ventana; // documentos por ventana
  int doc_count;
};

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
class CInit {

private:
  CTopic<HashSizeVentana, HashSizeCementerio> topic;
  CMap<std::string, CVector<std::string>, Fd_String<std::string>, 13> m_cache;
  CVector<std::string> queue_ventana_actual;
  PreprocesadorCPP preprocesador;
  int total_docs = 0;
  Init init;
  int numVentana = 0;

private:
  std::vector<std::string>
  leer_documentos_de_carpeta(const std::string &carpeta_path);
  std::string leer_archivo(const std::string &file_path);

public:
  CInit(Init init);
  void reprocesar_ventana_completa();
  void run();
  void runtest(long long OBJETIVO_DOCUMENTOS);
  void print();
};

#endif

// IMPLEMENTACION

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
CInit<HashSizeVentana, HashSizeCementerio>::CInit(Init _init)
    : topic(_init.k, _init.bucket_size, _init.tokens_ventana,
            _init.documentos_ventana),
      init(_init) {}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
std::vector<std::string>
CInit<HashSizeVentana, HashSizeCementerio>::leer_documentos_de_carpeta(
    const std::string &carpeta_path) {
  std::vector<std::string> archivos;

  try {
    if (!fs::exists(carpeta_path)) {
      std::cerr << "Error: La carpeta '" << carpeta_path << "' no existe."
                << std::endl;
      return archivos;
    }

    for (const auto &entry : fs::directory_iterator(carpeta_path)) {
      if (entry.is_regular_file()) {
        std::string extension = entry.path().extension().string();
        // Solo procesar archivos de texto
        if (extension == ".txt" || extension == ".text" || extension == "") {
          archivos.push_back(entry.path().string());
        }
      }
    }

    std::cout << "Encontrados " << archivos.size() << " archivos en '"
              << carpeta_path << "'" << std::endl;

  } catch (const fs::filesystem_error &ex) {
    std::cerr << "Error accediendo a la carpeta: " << ex.what() << std::endl;
  }

  return archivos;
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
std::string CInit<HashSizeVentana, HashSizeCementerio>::leer_archivo(
    const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo '" << file_path << "'"
              << std::endl;
    return "";
  }

  std::string contenido;
  std::string linea;
  while (std::getline(file, linea)) {
    contenido += linea + " ";
  }

  file.close();
  return contenido;
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::run() {

  std::string carpeta_docs = "docs";
  std::vector<std::string> archivos = leer_documentos_de_carpeta(carpeta_docs);
  if (archivos.empty()) {
    std::cout << "No se encontraron archivos en la carpeta 'docs'. Creando "
                 "archivos de ejemplo..."
              << std::endl;

    // Crear carpeta docs si no existe
    fs::create_directory("docs");

    // Crear algunos archivos de ejemplo
    std::vector<std::string> documentos_ejemplo = {
        "Rollins Goes 0-for-4 as Streak Ends PHILADELPHIA -- Jimmy Rollins was "
        "heading back to the clubhouse when Charlie Manuel put his arm around "
        "him and offered some encouraging words.",
        "Mets Beat Phillies 5-2 Behind Strong Pitching NEW YORK -- The New "
        "York Mets defeated the Philadelphia Phillies 5-2 on Saturday night.",
        "Yankees Win World Series in Game 7 Thriller NEW YORK -- The New York "
        "Yankees won their 27th World Series championship with a dramatic Game "
        "7 victory.",
        "Phillies Sign Free Agent Pitcher to 3-Year Deal PHILADELPHIA -- The "
        "Phillies have signed right-handed pitcher to a three-year contract "
        "worth $30 million.",
        "Eagles Prepare for Cowboys in NFC East Showdown PHILADELPHIA -- The "
        "Philadelphia Eagles are getting ready to face the Dallas Cowboys in a "
        "crucial division game."};

    for (size_t i = 0; i < documentos_ejemplo.size(); i++) {
      std::string nombre_archivo =
          "docs/documento_" + std::to_string(i + 1) + ".txt";
      std::ofstream file(nombre_archivo);
      if (file.is_open()) {
        file << documentos_ejemplo[i];
        file.close();
        archivos.push_back(nombre_archivo);
        std::cout << "Creado archivo de ejemplo: " << nombre_archivo
                  << std::endl;
      }
    }
  }
  for (std::size_t i = 0; i < archivos.size(); i++) {
    init.doc_count++;
    std::cout << "\n=== PROCESANDO DOCUMENTO " << init.doc_count
              << " ===" << std::endl;
    std::string texto_completo = leer_archivo(archivos[i]);

    if (texto_completo.empty()) {
      continue;
    }

    CVector<std::string> v_tokens =
        preprocesador.preprocesar_texto(texto_completo);

    std::string nameDoc = "doc_" + to_string(init.doc_count);
    m_cache.ins(nameDoc, v_tokens);
    queue_ventana_actual.push_back(nameDoc);

    for (int i = 0; i < v_tokens.size(); i++) {
      topic.add_ventana(v_tokens[i]);
    }
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
    }

    if (init.doc_count >= init.documentos_ventana) {
      std::string doc = queue_ventana_actual[0];
      queue_ventana_actual.pop_front();
      CVector<std::string> tokens_rem = m_cache.find(doc);
      topic.rem_freq(tokens_rem);
      numVentana++;
    }
  }
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::runtest(
    long long OBJETIVO_DOCUMENTOS) {
  std::string carpeta_docs = "docs";
  std::vector<std::string> archivos_reales =
      leer_documentos_de_carpeta(carpeta_docs);

  if (archivos_reales.empty()) {
    // Lógica para crear archivos de ejemplo si no existen (sin cambios)
    std::cout << "No se encontraron archivos en la carpeta 'docs'. Creando "
                 "archivos de ejemplo..."
              << std::endl;
    fs::create_directory("docs");
    std::vector<std::string> documentos_ejemplo = {
        "Rollins Goes 0-for-4 as Streak Ends PHILADELPHIA -- Jimmy Rollins was "
        "heading back to the clubhouse when Charlie Manuel put his arm around "
        "him and offered some encouraging words.",
        "Mets Beat Phillies 5-2 Behind Strong Pitching NEW YORK -- The New "
        "York Mets defeated the Philadelphia Phillies 5-2 on Saturday night.",
        "Yankees Win World Series in Game 7 Thriller NEW YORK -- The New York "
        "Yankees won their 27th World Series championship with a dramatic Game "
        "7 victory.",
        "Phillies Sign Free Agent Pitcher to 3-Year Deal PHILADELPHIA -- The "
        "Phillies have signed right-handed pitcher to a three-year contract "
        "worth $30 million.",
        "Eagles Prepare for Cowboys in NFC East Showdown PHILADELPHIA -- The "
        "Philadelphia Eagles are getting ready to face the Dallas Cowboys in a "
        "crucial division game."};

    for (size_t i = 0; i < documentos_ejemplo.size(); i++) {
      std::string nombre_archivo =
          "docs/documento_" + std::to_string(i + 1) + ".txt";
      std::ofstream file(nombre_archivo);
      if (file.is_open()) {
        file << documentos_ejemplo[i];
        file.close();
        archivos_reales.push_back(nombre_archivo);
        std::cout << "Creado archivo de ejemplo: " << nombre_archivo
                  << std::endl;
      }
    }
  }

  // --- Lógica principal para procesar 1 millón de documentos ---

  size_t num_archivos_reales = archivos_reales.size();

  if (num_archivos_reales == 0) {
    std::cerr << "Error: No hay archivos reales para procesar." << std::endl;
    return;
  }

  std::cout << "Iniciando procesamiento de hasta " << OBJETIVO_DOCUMENTOS
            << " documentos (reutilizando " << num_archivos_reales
            << " archivos reales)." << std::endl;

  // Usamos un bucle while que se ejecuta hasta alcanzar 1 millón de documentos.
  while (init.doc_count < OBJETIVO_DOCUMENTOS) {

    // Iteramos sobre cada archivo real disponible
    for (size_t i = 0; i < num_archivos_reales; ++i) {

      // Si ya alcanzamos el millón dentro del bucle interno, salimos
      if (init.doc_count >= OBJETIVO_DOCUMENTOS) {
        break;
      }

      // 1. Incrementar el contador global del documento simulado
      init.doc_count++;

      // 2. Identificar el archivo real actual (ciclicamente)
      std::string ruta_archivo_actual = archivos_reales[i];

      // 3. Generar un nombre de documento único para la caché (ej: "doc_1",
      // "doc_1000000")
      std::string nameDocCache = "doc_" + std::to_string(init.doc_count);

      std::cout << "\n=== PROCESANDO DOCUMENTO SIMULADO " << init.doc_count
                << " (Archivo real: " << ruta_archivo_actual
                << ") ===" << std::endl;

      // 4. Leer y preprocesar el archivo real (desde disco o caché si tuvieras
      // esa lógica)
      std::string texto_completo = leer_archivo(ruta_archivo_actual);

      if (texto_completo.empty()) {
        continue;
      }

      // 8. Lógica de ventana deslizante: si la ventana está llena, se quita el
      // documento más antiguo

      if (init.doc_count >= init.documentos_ventana) {
        // std::cout << VERDE;
        // topic.printVentanaActual();
        // std::cout << RESET;
        std::string doc_a_remover = queue_ventana_actual[0];
        queue_ventana_actual.pop_front();
        CVector<std::string> tokens_rem = m_cache.find(doc_a_remover);
        topic.rem_freq(tokens_rem);
        numVentana++;
        // std::cout << ROJO << "==== VENTANA DESLIZADA (" << numVentana
        //           << "). Removido: " << doc_a_remover << " ====" << RESET
        //           << std::endl;
      }

      CVector<std::string> v_tokens =
          preprocesador.preprocesar_texto(texto_completo);

      // 5. Insertar en caché con el nombre *único* del documento simulado
      m_cache.ins(nameDocCache, v_tokens);
      queue_ventana_actual.push_back(nameDocCache);

      // 6. Añadir tokens a la ventana actual del topic model
      // cout << "Tokens:[";
      for (int t = 0; t < v_tokens.size(); t++) { // CAMBIARA QUI
        // cout << v_tokens[t] << ", ";
        topic.add_ventana(v_tokens[t]);
      }
      // cout << "]\n";
      //  7. Añadir al cementerio (estructura topic para mantener estadísticas
      //  globales)
      for (std::size_t c = 0; c < v_tokens.size(); c++) {
        topic.add_cementerio(v_tokens[c]);
      }
    }
  }

  std::cout << "Procesamiento completado. Total de documentos simulados: "
            << init.doc_count << std::endl;

  // Nota: El código original terminaba aquí sin un bucle final para ventanas
  // restantes, asumiendo que el bucle while principal manejaba toda la lógica.
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::print() {
  topic.printVentanaActual();
}