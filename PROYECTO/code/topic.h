#include "hashmap.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

template <class T> struct Fd {
  inline unsigned long operator()(const T &x) const {
    unsigned long hash_value = 0;

    int i = 1;
    for (char c : x.topic) {
      hash_value += static_cast<unsigned long>(c) * i + i;
      i++;
    }
    return hash_value;
  }
};

template <unsigned long Sv, unsigned long Sc> class CTopic {
private:
  CHashMap<Data, Fd<Data>, Sv> h_ventana;
  CHashMap<Data, Fd<Data>, Sc> h_cementerio;

  int num_doc_ventana;    // numero de documentos por ventana
  int num_ventana_actual; // numero de ventana actual
  int tokens_ventana;     // numero de tokens por ventana
  int num_poda_ventana;   // contador de podas para esta ventana
  int k;                  // k treding por vetana
  int bucket_size;        // cada cuanto hacer poda

private:
  void ejecutar_poda();             // Ejecuta poda en ventana actual
  bool debe_ser_podado(Node *nodo); // Decide si un nodo debe podarse
  vector<string> obtener_top_k();   // Devuelve top-K trending actual
  int obtener_frecuencia_historica(string palabra); // Consulta histórica
  void limpiar_ventana_actual();

public:
  CTopic(int k, int bucket_size, int tokens_ventana, int num_doc_ventana);
  void add_cementerio(string token);
  void add_ventana(string token);
  void iniciar_nueva_ventana();
  void printVentanaActual();
};

//*************************************************** */
//                  IMPLEMENTACION                     /
//*************************************************** */

template <unsigned long Sv, unsigned long Sc>
CTopic<Sv, Sc>::CTopic(int _k, int _bs, int _tv, int _dv) {
  k = _k;
  bucket_size = _bs;
  num_poda_ventana = 0;
  num_ventana_actual = 0;
  num_doc_ventana = _dv;
  tokens_ventana = _tv;
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::add_ventana(string token) {
  Data d = {token, 1, 0};
  h_ventana.ins(d);

  tokens_ventana++;
  if (tokens_ventana % bucket_size == 0) {
    ejecutar_poda();
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::add_cementerio(string token) {
  Data d = {token, 1, 0};
  h_cementerio.ins(d);
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::iniciar_nueva_ventana() {
  for (int i = 0; i < Sv; i++) {
    h_ventana.bucket[i].limpiar_lista();
  }

  // Reiniciar contadores de ventana
  tokens_ventana = 0;
  num_poda_ventana = 0;
  num_ventana_actual++;
}
template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::ejecutar_poda() {}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::printVentanaActual() {
  h_ventana.PrintTable();
}