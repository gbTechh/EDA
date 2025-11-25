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
  void run(int TIMESLEEP_MS = 100);
  void runtest(long long OBJETIVO_DOCUMENTOS, int TIMESLEEP_MS);
  void print();
};

#endif

// IMPLEMENTACION

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
CInit<HashSizeVentana, HashSizeCementerio>::CInit(Init _init)
    : topic(_init.k, _init.bucket_size, _init.documentos_ventana), init(_init) {
  preprocesador.cargar_diccionario("lematizacion.txt");
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
void CInit<HashSizeVentana, HashSizeCementerio>::run(int TIMESLEEP_MS) {

  std::string carpeta_docs = "docs";
  std::vector<std::string> archivos = leer_documentos_de_carpeta(carpeta_docs);
  if (archivos.empty()) {
    std::cout << "No se encontraron archivos en la carpeta 'docs'. Creando "
                 "archivos de ejemplo..."
              << std::endl;

    return;
  }
  for (std::size_t i = 0; i < archivos.size(); i++) {
    init.doc_count++;
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
      topic.add_ventana(v_tokens[i], TIMESLEEP_MS);
    }
    for (std::size_t c = 0; c < v_tokens.size(); c++) {
      topic.add_cementerio(v_tokens[c]);
    }
  }
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::runtest(
    long long OBJETIVO_DOCUMENTOS, int TIMESLEEP_MS) {
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

      m_cache.ins(nameDocCache, v_tokens);
      queue_ventana_actual.push(nameDocCache);

      for (std::size_t t = 0; t < v_tokens.size(); t++) { // CAMBIARA QUI
        topic.add_ventana(v_tokens[t], TIMESLEEP_MS);
      }
      for (std::size_t c = 0; c < v_tokens.size(); c++) {
        topic.add_cementerio(v_tokens[c]);
      }
    }
  }

  std::cout << "Procesamiento completado. Total de documentos simulados: "
            << init.doc_count << std::endl;
}

template <unsigned long HashSizeVentana, unsigned long HashSizeCementerio>
void CInit<HashSizeVentana, HashSizeCementerio>::print() {
  topic.printVentanaActual();
}