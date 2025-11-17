#ifndef MAP_H
#define MAP_H

#include "listamap.h"
#include <iostream>
#include <stdexcept>

template <class K, class V, class Fd, unsigned long Sz> class CMap {
public:
  CListMap<K, V> bucket[Sz];
  Fd fd;
  int num_podas = 0;

public:
  CMap() = default;
  // ~CMap();

  void ins(K k, V v);
  bool findk(K k); // buscar key
  V find(K k);     // buscar key y devolver su value
  bool rem(K k);
  void PrintTable() const;
};

//*************************************************** */
//                  IMPLEMENTACION                     /
//*************************************************** */

template <class K, class V, class Fd, unsigned long Sz>
void CMap<K, V, Fd, Sz>::ins(K k, V v) {
  bucket[fd(k) % Sz].Ins({k, v});
}

template <class K, class V, class Fd, unsigned long Sz>
bool CMap<K, V, Fd, Sz>::findk(K k) {
  unsigned long bucket_index = fd(k) % Sz;
  auto &list_ref = bucket[bucket_index];

  const KeyValue<K, V> *result = list_ref.get(k);
  return result != nullptr;
}

template <class K, class V, class Fd, unsigned long Sz>
V CMap<K, V, Fd, Sz>::find(K k) {
  unsigned long bucket_index = fd(k) % Sz;
  auto &list_ref = bucket[bucket_index];

  const KeyValue<K, V> *result = list_ref.get(k);

  if (result != nullptr) {
    return result->value;
  } else {
    throw std::out_of_range("CMap::find: Key not found");
  }
}

template <class K, class V, class Fd, unsigned long Sz>
bool CMap<K, V, Fd, Sz>::rem(K k) {
  unsigned long bucket_index = fd(k) % Sz;
  return bucket[bucket_index].Rem(k);
}

template <class K, class V, class Fd, unsigned long Sz>
void CMap<K, V, Fd, Sz>::PrintTable() const {
  std::cout << "\n--- ESTADO DE LA TABLA HASH (Tamano: " << Sz << ") ---\n";
  for (unsigned long i = 0; i < Sz; ++i) {
    std::cout << "[" << i << "]";
    if (bucket[i].size > 0) {
      std::cout << " -> ";
      bucket[i].PrintList();
    } else {
      std::cout << " (vacio)";
    }
    std::cout << "\n";
  }
  std::cout << "------------------------------------------------------\n";
}
#endif