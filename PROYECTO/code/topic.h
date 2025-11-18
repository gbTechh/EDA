#ifndef TOPIC_H
#define TOPIC_H

#include "hashmap.h"
#include "minaux.h"
#include "vector.h"
#include <iostream>
#include <string>

using namespace std;

template <class T> struct Fd {
  inline unsigned long operator()(const T &x) const {
    // FNV-1a - Mucho mejor distribución
    unsigned long hash = 14695981039346656037UL;
    for (char c : x.topic) {
      hash ^= static_cast<unsigned long>(c);
      hash *= 1099511628211UL;
    }
    return hash;
  }
};

template <class T> struct FdMap {
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

template <unsigned long Sv, unsigned long Sc> class CTopic {
private:
  CHashMap<Data, CList, Fd<Data>, Sv> h_ventana;
  CHashMap<Data, CList, Fd<Data>, Sc> h_cementerio;
  CHeap<Data> heap;

  int num_doc_ventana;    // numero de documentos por ventana
  int num_ventana_actual; // numero de ventana actual
  int tokens_ventana = 0; // numero de tokens por ventana
  int num_poda_ventana;   // contador de podas para esta ventana
  int k;                  // k treding por vetana
  int bucket_size;        // cada cuanto hacer poda

  // ========== CONTADORES DE DEBUG ==========
  long long total_remFreq_calls = 0;
  long long total_remFreq_tokens_processed = 0;
  long long total_remFreq_successful = 0;
  long long total_heap_updates = 0;
  long long total_add_ventana_calls = 0;
  long long total_podas_ejecutadas = 0;

private:
  void ejecutar_poda();             // Ejecuta poda en ventana actual
  bool debe_ser_podado(Node *nodo); // Decide si un nodo debe podarse
  int obtener_frecuencia_historica(string palabra); // Consulta histórica
  void limpiar_ventana_actual();
  void actualizar_heap(std::string token);
  void printDebugStats();

public:
  CTopic(int k, int bucket_size, int num_doc_ventana);
  void add_cementerio(string token);
  void add_ventana(string token);
  void rem_freq(CVector<std::string>);
  void iniciar_nueva_ventana();
  void printVentanaActual();
};

//*************************************************** */
//                  IMPLEMENTACION                     /
//*************************************************** */

template <unsigned long Sv, unsigned long Sc>
CTopic<Sv, Sc>::CTopic(int _k, int _bs, int _dv) {
  k = _k;
  bucket_size = _bs;
  num_poda_ventana = 0;
  num_ventana_actual = 0;
  num_doc_ventana = _dv;
  tokens_ventana = 0;
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::add_ventana(string token) {
  total_add_ventana_calls++;
  Data d = {token, 1, num_poda_ventana};
  ++tokens_ventana;
  h_ventana.ins(d);
  // cout << "Data: " << d.frq << " - " << d.topic << "\n";
  if (heap.size() < k) {
    heap.push(d);
  } else {
    Data data = heap.top();
    if (d.frq >= heap.get_min_frequency()) {
      heap.pop();
      heap.push(d);
    }
  }
  // heap.print();

  if (tokens_ventana % bucket_size == 0) {

    ejecutar_poda();
    num_poda_ventana++;
    total_podas_ejecutadas++;
    // cout << "NUM PODA: " << num_poda_ventana << endl;
    //  h_ventana.PrintTable();
  }
  if (total_add_ventana_calls % 1000 == 0) {
    // printDebugStats();
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::add_cementerio(string token) {
  Data d = {token, 1, 0};
  h_cementerio.ins(d);
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::actualizar_heap(string token) {
  Data d = {token, 0, 0};

  // Solo actualizar si vale la pena
  if (h_ventana.search(d) && d.frq >= 2) {
    int min_freq = heap.get_min_frequency();

    // Estrategia más inteligente:
    if (d.frq >= min_freq || heap.size() < k) {
      if (heap.exists(token)) {
        heap.update(token, d.frq);
      } else if (heap.size() < k || d.frq > min_freq) {
        heap.push(d);
      }
    }
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::iniciar_nueva_ventana() {
  for (int i = 0; i < Sv; i++) {
    h_ventana.bucket[i].limpiar_lista();
  }

  while (!heap.empty()) {
    heap.pop();
  }

  // Reiniciar contadores de ventana
  tokens_ventana = 0;
  num_poda_ventana = 0;
  num_ventana_actual++;
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::ejecutar_poda() {
  int elementos_eliminados = 0;

  for (int i = 0; i < Sv; i++) {
    auto &it = h_ventana.bucket[i];
    Node *current = it.root;
    Node *previous = nullptr;

    while (current != nullptr) {
      Node *next = current->next;

      // PODAR MÁS AGRESIVAMENTE
      bool debe_podar = current->data.error + current->data.frq <=
                            num_poda_ventana ||   // Lossy Counting
                        current->data.frq <= 1 || // Frecuencia baja
                        (num_poda_ventana % 5 == 0 &&
                         current->data.frq <= 2); // Limpieza periódica

      if (debe_podar) {
        if (previous == nullptr) {
          it.root = next;
        } else {
          previous->next = next;
        }
        delete current;
        elementos_eliminados++;
        it.size--;
      } else {
        previous = current;
      }
      current = next;
    }
  }
}
template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::rem_freq(CVector<std::string> v_tokens) {
  total_remFreq_calls++;
  total_remFreq_tokens_processed += v_tokens.size();
  for (int i = 0; i < v_tokens.size(); i++) {
    Data d = {v_tokens[i], 0, 0};

    if (h_ventana.remFreq(d)) {
      if (d.frq > 0) {
        actualizar_heap(v_tokens[i]);
      } else {
        if (heap.exists(v_tokens[i])) {
        }
      }
    }
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::printVentanaActual() {
  // h_ventana.PrintTable();
  heap.print();
}
// ========== NUEVA FUNCIÓN DE DEBUG ==========
template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::printDebugStats() {
  std::cout << "\n=== DEBUG STATS ===" << std::endl;
  std::cout << "Total add_ventana calls: " << total_add_ventana_calls
            << std::endl;
  std::cout << "Total rem_freq calls: " << total_remFreq_calls << std::endl;
  std::cout << "Total tokens procesados en rem_freq: "
            << total_remFreq_tokens_processed << std::endl;
  std::cout << "Total remFreq exitosos: " << total_remFreq_successful
            << std::endl;
  std::cout << "Total actualizaciones de heap: " << total_heap_updates
            << std::endl;
  std::cout << "Total podas ejecutadas: " << total_podas_ejecutadas
            << std::endl;
  std::cout << "Tamaño actual del heap: " << heap.size() << std::endl;
  std::cout << "Frecuencia mínima en heap: " << heap.get_min_frequency()
            << std::endl;
  std::cout << "Tokens en ventana: " << tokens_ventana << std::endl;
  std::cout << "Num poda ventana: " << num_poda_ventana << std::endl;

  // Calcular ratios
  if (total_remFreq_tokens_processed > 0) {
    double success_ratio =
        (double)total_remFreq_successful / total_remFreq_tokens_processed * 100;
    std::cout << "Ratio éxito rem_freq: " << success_ratio << "%" << std::endl;
  }

  if (total_add_ventana_calls > 0) {
    double heap_update_ratio =
        (double)total_heap_updates / total_add_ventana_calls * 100;
    std::cout << "Ratio updates heap: " << heap_update_ratio << "%"
              << std::endl;
  }
  std::cout << "===================\n" << std::endl;
}

#endif