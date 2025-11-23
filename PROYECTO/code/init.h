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
#include <queue>
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
  int documentos_ventana; // documentos por ventana
  int doc_count;
};

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
class CInit {

private:
  CTopic<HashSizeVentana, HashSizeCementerio> topic;
  CMap<std::string, CVector<std::string>, Fd_String<std::string>, 13> m_cache;
  queue<std::string> queue_ventana_actual;
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
  void runtestvector(const CVector<CVector<std::string>> &documentos_test);
  void print();
  void runtestvector2(const CVector<CVector<std::string>> &documentos_test);
};

#endif

// IMPLEMENTACION

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
CInit<HashSizeVentana, HashSizeCementerio>::CInit(Init _init)
    : topic(_init.k, _init.bucket_size, _init.documentos_ventana), init(_init) {
}

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
    // std::cout << "\n=== PROCESANDO DOCUMENTO " << init.doc_count << " ===" <<
    // std::endl;
    std::string texto_completo = leer_archivo(archivos[i]);

    if (texto_completo.empty()) {
      continue;
    }

    if (init.doc_count >= init.documentos_ventana) {
      std::string doc = queue_ventana_actual.front();
      queue_ventana_actual.pop();
      CVector<std::string> tokens_rem = m_cache.find(doc);
      topic.rem_freq(tokens_rem);
      numVentana++;
    }

    CVector<std::string> v_tokens =
        preprocesador.preprocesar_texto(texto_completo);

    std::string nameDoc = "doc_" + to_string(init.doc_count);
    m_cache.ins(nameDoc, v_tokens);
    queue_ventana_actual.push(nameDoc);

    for (std::size_t i = 0; i < v_tokens.size(); i++) {
      topic.add_ventana(v_tokens[i]);
    }
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
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
    std::cout << "No se encontraron archivos en la carpeta 'docs'."
              << std::endl;
    return;
  }

  size_t num_archivos_reales = archivos_reales.size();

  if (num_archivos_reales == 0) {
    std::cerr << "Error: No hay archivos reales para procesar." << std::endl;
    return;
  }

  std::cout << "Iniciando procesamiento de hasta " << OBJETIVO_DOCUMENTOS
            << " documentos (reutilizando " << num_archivos_reales
            << " archivos reales)." << std::endl;

  while (init.doc_count < OBJETIVO_DOCUMENTOS) {

    for (size_t i = 0; i < num_archivos_reales; ++i) {

      if (init.doc_count >= OBJETIVO_DOCUMENTOS) {
        break;
      }

      init.doc_count++;

      std::string ruta_archivo_actual = archivos_reales[i];
      std::string nameDocCache = "doc_" + std::to_string(init.doc_count);
      std::string texto_completo = leer_archivo(ruta_archivo_actual);

      if (texto_completo.empty()) {
        continue;
      }

      if (init.doc_count >= init.documentos_ventana) {
        // std::cout << VERDE;
        // topic.printVentanaActual();
        // std::cout << RESET;
        std::string doc_a_remover = queue_ventana_actual.front();
        queue_ventana_actual.pop();
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
      queue_ventana_actual.push(nameDocCache);

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
void CInit<HashSizeVentana, HashSizeCementerio>::runtestvector(
    const CVector<CVector<std::string>> &documentos_test) {

  std::cout << "🧪 INICIANDO TEST CON VECTOR PREDEFINIDO" << std::endl;
  std::cout << "Documentos a procesar: " << documentos_test.size() << std::endl;

  // Reiniciar contadores para test limpio
  init.doc_count = 0;
  numVentana = 0;

  // Procesar cada documento del vector de test
  for (int doc_idx = 0; doc_idx < documentos_test.size(); doc_idx++) {
    init.doc_count++;

    std::cout << "\n=== PROCESANDO DOCUMENTO TEST " << init.doc_count
              << " ===" << std::endl;

    const CVector<std::string> &v_tokens = documentos_test[doc_idx];

    std::cout << "Tokens: [";
    for (int t = 0; t < v_tokens.size(); t++) {
      std::cout << v_tokens[t];
      if (t < v_tokens.size() - 1)
        std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // Usar nombre de documento único para la caché
    std::string nameDoc = "test_doc_" + std::to_string(init.doc_count);
    m_cache.ins(nameDoc, v_tokens);
    queue_ventana_actual.push(nameDoc);

    // Procesar tokens en el topic model
    for (int t = 0; t < v_tokens.size(); t++) {
      topic.add_ventana(v_tokens[t]);
    }

    // Añadir al cementerio
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
    }

    // Lógica de ventana deslizante
    if (init.doc_count >= init.documentos_ventana) {
      std::string doc_a_remover = queue_ventana_actual.front();
      queue_ventana_actual.pop();
      CVector<std::string> tokens_rem = m_cache.find(doc_a_remover);

      std::cout << "🔄 VENTANA DESLIZADA - Removiendo: " << doc_a_remover
                << " con " << tokens_rem.size() << " tokens" << std::endl;

      topic.rem_freq(tokens_rem);
      numVentana++;

      // Mostrar estado después de rem_freq
      std::cout << "Estado después de rem_freq: ";
      topic.printVentanaActual();
    }

    // Mostrar estado actual del heap
    std::cout << "Heap actual (doc " << init.doc_count << "): ";
    topic.printVentanaActual();
  }

  std::cout << "\n🎉 TEST CON VECTOR COMPLETADO" << std::endl;
  std::cout << "Total documentos procesados: " << init.doc_count << std::endl;
  std::cout << "Total ventanas deslizadas: " << numVentana << std::endl;
  std::cout << "RESULTADO FINAL - Top " << init.k
            << " trending topics:" << std::endl;
  topic.printVentanaActual();
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::runtestvector2(
    const CVector<CVector<std::string>> &documentos_test) {

  std::cout << "🧪 INICIANDO TEST CON VECTOR PREDEFINIDO" << std::endl;
  std::cout << "Documentos a procesar: " << documentos_test.size() << std::endl;
  std::cout << "Tamaño de ventana: " << init.documentos_ventana << std::endl;

  // Reiniciar contadores para test limpio
  init.doc_count = 0;
  numVentana = 0;
  // Limpiar estructuras
  while (!queue_ventana_actual.empty())
    queue_ventana_actual.pop();
  m_cache =
      CMap<std::string, CVector<std::string>, Fd_String<std::string>, 13>();

  std::cout << "\n=== CONFIGURACIÓN INICIAL ===" << std::endl;
  std::cout << "K (topics): " << init.k << std::endl;
  std::cout << "Bucket size: " << init.bucket_size << std::endl;

  // Procesar cada documento del vector de test
  for (int doc_idx = 0; doc_idx < documentos_test.size(); doc_idx++) {
    init.doc_count++;

    std::cout << "\n=== PROCESANDO DOCUMENTO TEST " << init.doc_count
              << " ===" << std::endl;

    const CVector<std::string> &v_tokens = documentos_test[doc_idx];

    std::cout << "Tokens: [";
    for (int t = 0; t < v_tokens.size(); t++) {
      std::cout << v_tokens[t];
      if (t < v_tokens.size() - 1)
        std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // Usar nombre de documento único para la caché
    std::string nameDoc = "test_doc_" + std::to_string(init.doc_count);
    m_cache.ins(nameDoc, v_tokens);
    queue_ventana_actual.push(nameDoc);

    // Procesar tokens en el topic model
    for (int t = 0; t < v_tokens.size(); t++) {
      topic.add_ventana(v_tokens[t]);
    }

    // Añadir al cementerio
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
    }

    // Lógica de ventana deslizante
    if (init.doc_count >= init.documentos_ventana) {
      std::string doc_a_remover = queue_ventana_actual.front();
      queue_ventana_actual.pop();
      CVector<std::string> tokens_rem = m_cache.find(doc_a_remover);

      std::cout << "🔄 VENTANA DESLIZADA - Removiendo: " << doc_a_remover
                << " con " << tokens_rem.size() << " tokens" << std::endl;

      std::cout << "Tokens a remover: [";
      for (int t = 0; t < tokens_rem.size(); t++) {
        std::cout << tokens_rem[t];
        if (t < tokens_rem.size() - 1)
          std::cout << ", ";
      }
      std::cout << "]" << std::endl;

      topic.rem_freq(tokens_rem);
      numVentana++;

      // Mostrar estado después de rem_freq
      std::cout << "Estado después de rem_freq: ";
      topic.printVentanaActual();
    }

    // Mostrar estado actual del heap
    std::cout << "Heap actual (doc " << init.doc_count << "): ";
    topic.printVentanaActual();
  }

  std::cout << "\n🎉 TEST CON VECTOR COMPLETADO" << std::endl;
  std::cout << "Total documentos procesados: " << init.doc_count << std::endl;
  std::cout << "Total ventanas deslizadas: " << numVentana << std::endl;
  std::cout << "RESULTADO FINAL - Top " << init.k
            << " trending topics:" << std::endl;
  topic.printVentanaActual();

  // Mostrar estadísticas de debug del topic
  // topic.printDebugStats(); // Descomenta si tienes esta función
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::print() {
  topic.printVentanaActual();
}