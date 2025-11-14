#include "topic.h"
#include <deque>
#include <iostream>

struct Init {
  int k;
  int bucket_size;
  int tokens_ventana;
  int documentos_ventana;
  int doc_procesados;
};

class CInit {

private:
  CTopic<227, 300> topic;
  deque<vector<string>> deque_ventana_actual;
  int total_docs = 0;
  Init init;

public:
  CInit(Init init);
  void procesar_documento(const vector<string> &documento);
  void reprocesar_ventana_completa();
  void run();
  void print();
};