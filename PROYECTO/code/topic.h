#ifndef TOPIC_H
#define TOPIC_H

#include "hashmap.h"
#include "map.h"
#include "minheapindice.h"
#include "vector.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

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

struct DataIndices {
  std::string topic;
  int index;
};
// sv = size ventana
// sc = size cementerio
template <unsigned long Sv, unsigned long Sc> class CTopic {
private:
  CHashMap<Data, CList, Fd<Data>, Sv> h_ventana;
  CHashMap<Data, CList, Fd<Data>, Sc> h_cementerio;
  CMinHeapInd<Data> heap;

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

  int export_counter = 0;

private:
  void ejecutar_poda(); // Ejecuta poda en ventana actual
  void rellenar_heap();
  void verificar_heap_despues_reduccion();
  void printDebugStats();

public:
  CTopic(int k, int bucket_size, int num_doc_ventana);
  void add_cementerio(string token);
  void add_ventana(string token, int ms);
  void rem_freq(CVector<std::string> v_tokens);
  void iniciar_nueva_ventana();
  CVector<std::string> get_k_topics();
  void printVentanaActual();
  void exportar_wordcloud_json(const std::string &filename);
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
void CTopic<Sv, Sc>::add_ventana(string token, int ms) {
  total_add_ventana_calls++;
  Data d = {token, 1, num_poda_ventana};
  h_ventana.ins(d);
  ++tokens_ventana;

  bool heap_changed = false;

  if (heap.exists(token)) {
    heap.update(token, d.frq);
    total_heap_updates++;
    heap_changed = true;
  } else {
    if (heap.size() < static_cast<std::size_t>(k)) {
      heap.push(d);
      heap_changed = true;
    } else {
      int min_freq = heap.get_min_frequency();
      if (d.frq > min_freq) {
        heap.pop();
        heap.push(d);
        total_heap_updates++;
        heap_changed = true;
      }
    }
  }
  // exprotar json solo si heap cambio
  if (heap_changed) {
    exportar_wordcloud_json("wordcloud_data.json");

    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    std::cout << "\r📊 Tokens: " << tokens_ventana << " | Heap: " << heap.size()
              << " | Top: ";

    auto heap_data = heap.get_heap_data();
    int count = 0;
    for (std::size_t i = 0; i < heap_data.size() && count < 3; i++) {
      std::cout << heap_data[i].topic << "(" << heap_data[i].frq << ") ";
      count++;
    }
    std::cout << "          " << std::flush;
  }

  if (tokens_ventana % bucket_size == 0) {
    ejecutar_poda();
    num_poda_ventana++;
    total_podas_ejecutadas++;

    exportar_wordcloud_json("wordcloud_data.json");
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::add_cementerio(string token) {
  Data d = {token, 1, 0};
  h_cementerio.ins(d);
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::rem_freq(CVector<std::string> v_tokens) {
  total_remFreq_calls++;
  total_remFreq_tokens_processed += v_tokens.size();

  for (std::size_t i = 0; i < v_tokens.size(); i++) {
    Data d_to_remove = {v_tokens[i], 0, 0};

    if (h_ventana.remFreq(d_to_remove)) {
      int nueva_frecuencia = d_to_remove.frq;
      if (heap.exists(v_tokens[i])) {
        if (nueva_frecuencia > 0) {
          heap.update(v_tokens[i], nueva_frecuencia);
        } else {
          heap.remove(v_tokens[i]);
        }
      }
    }
  }
  verificar_heap_despues_reduccion();
}

// Verificar si hay palabras fuera del heap con mayor frecuencia
template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::verificar_heap_despues_reduccion() {
  if (heap.size() < static_cast<std::size_t>(k)) {
    rellenar_heap();
    return;
  }

  // Si está lleno, buscar candidatos mejores
  int min_freq = heap.get_min_frequency();
  Data mejor_candidato = {"", 0, 0};
  bool hay_candidato = false;

  for (unsigned long i = 0; i < Sv; i++) {
    Node *current = h_ventana.bucket[i].root;

    while (current != nullptr) {
      string palabra = current->data.topic;
      int freq = current->data.frq;

      if (!heap.exists(palabra) && freq > min_freq) {
        if (!hay_candidato || freq > mejor_candidato.frq) {
          mejor_candidato = current->data;
          hay_candidato = true;
        }
      }
      current = current->next;
    }
  }

  // Si encontramos un mejor candidato, reemplazar
  if (hay_candidato) {
    heap.pop();
    heap.push(mejor_candidato);
  }
}

// Rellenar heap cuando tiene < k elementos
template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::rellenar_heap() {
  int espacios_libres = k - heap.size();

  if (espacios_libres <= 0)
    return;

  // Recolectar candidatos (palabras NO en heap)
  CVector<Data> candidatos;

  for (unsigned long i = 0; i < Sv; i++) {
    Node *current = h_ventana.bucket[i].root;

    while (current != nullptr) {
      if (!heap.exists(current->data.topic)) {
        candidatos.push_back(current->data);
      }
      current = current->next;
    }
  }

  for (std::size_t i = 0;
       i < candidatos.size() && i < static_cast<std::size_t>(espacios_libres);
       i++) {
    int max_idx = i;
    for (std::size_t j = i + 1; j < candidatos.size(); j++) {
      if (candidatos[j].frq > candidatos[max_idx].frq) {
        max_idx = j;
      }
    }

    // Swap
    Data temp = candidatos[i];
    candidatos[i] = candidatos[max_idx];
    candidatos[max_idx] = temp;

    // Agregar al heap
    heap.push(candidatos[i]);
  }
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::ejecutar_poda() {
  int elementos_eliminados = 0;
  CVector<string> palabras_eliminadas;

  for (unsigned long i = 0; i < Sv; i++) {
    CList &lista = h_ventana.bucket[i];
    Node *current = lista.root;
    Node *previous = nullptr;

    while (current != nullptr) {
      Node *next = current->next;

      bool debe_podar =
          (current->data.frq + current->data.error) <= num_poda_ventana;

      if (debe_podar) {
        palabras_eliminadas.push_back(current->data.topic);

        if (previous == nullptr) {
          lista.root = next;
        } else {
          previous->next = next;
        }
        delete current;
        elementos_eliminados++;
        lista.size--;
      } else {
        previous = current;
      }
      current = next;
    }
  }

  // eliminar paplbras pdodass del heap
  for (std::size_t i = 0; i < palabras_eliminadas.size(); i++) {
    if (heap.exists(palabras_eliminadas[i])) {
      heap.remove(palabras_eliminadas[i]);
    }
  }

  if (heap.size() < static_cast<std::size_t>(k)) {
    rellenar_heap();
  }
}

template <unsigned long Sv, unsigned long Sc>
CVector<std::string> CTopic<Sv, Sc>::get_k_topics() {
  CVector<std::string> v_topics;
  CMinHeapInd<Data> heap;
  int it = heap.size() - k;
  for (int i = 0; i < it; i++) {
    heap.pop();
  }
  Data d;
  for (int i = 0; i < k; i++) {
    d = heap.top();
    heap.pop();
    v_topics.push_back(d.topic);
  }
  return v_topics;
}

template <unsigned long Sv, unsigned long Sc>
void CTopic<Sv, Sc>::exportar_wordcloud_json(const std::string &filename) {
  std::ofstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Error: No se pudo abrir " << filename << std::endl;
    return;
  }

  const CVector<Data> &heap_data = heap.get_heap_data();

  file << "{\n";
  file << "  \"timestamp\": " << tokens_ventana << ",\n";
  file << "  \"ventana\": " << num_ventana_actual << ",\n";
  file << "  \"export_num\": " << export_counter++ << ",\n";
  file << "  \"topics\": [\n";

  // Copiar y ordenar por frecuencia
  CVector<Data> temp_data;
  for (std::size_t i = 0; i < heap_data.size(); i++) {
    temp_data.push_back(heap_data[i]);
  }

  // Selection sort descendente por frecuencia
  for (std::size_t i = 0; i < temp_data.size(); i++) {
    for (std::size_t j = i + 1; j < temp_data.size(); j++) {
      if (temp_data[j].frq > temp_data[i].frq) {
        Data t = temp_data[i];
        temp_data[i] = temp_data[j];
        temp_data[j] = t;
      }
    }
  }

  // Escribir JSON
  for (std::size_t i = 0; i < temp_data.size(); i++) {
    file << "    {\"word\": \"" << temp_data[i].topic
         << "\", \"frequency\": " << temp_data[i].frq << "}";

    if (i < temp_data.size() - 1) {
      file << ",";
    }
    file << "\n";
  }

  file << "  ]\n";
  file << "}\n";

  file.close();
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