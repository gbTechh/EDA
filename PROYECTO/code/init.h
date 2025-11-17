#ifndef INIT_H
#define INIT_H

#include "data.h"
#include "map.h"
#include "preprocesador_cpp.h"
#include "topic.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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

class CInit {

private:
  CTopic<9977, 100000> topic;
  CMap<std::string, CVector<std::string>, Fd_String<std::string>, 9977> m_cache;
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
  void procesar_ventana();
  void reprocesar_ventana_completa();
  void run();
  void runtest();
  void print();
};

#endif