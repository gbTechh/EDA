#include "lista.h"
#include <iostream>

template <class T, class Fd, unsigned long Sz> class CHashMap {
public:
  CList bucket[Sz];
  Fd fd;
  int num_podas = 0;

public:
  CHashMap() = default;
  // ~CHashMap();

  void ins(T v);
  bool find(T v);
  bool rem(T v);
  void PrintTable() const;
};

//*************************************************** */
//                  IMPLEMENTACION                     /
//*************************************************** */

// template <class T, class Fd, unsigned long Sz>
//  CHashMap<T, Fd, Sz>::CHashMap() {}

// template <class T, class Fd, unsigned long Sz>
// CHashMap<T, Fd, Sz>::~CHashMap() {
//   delete[] bucket;
// }

template <class T, class Fd, unsigned long Sz>
void CHashMap<T, Fd, Sz>::ins(T v) {
  bucket[fd(v) % Sz].Ins(v, 0);
}

template <class T, class Fd, unsigned long Sz>
bool CHashMap<T, Fd, Sz>::find(T v) {
  CList &l = bucket[fd(v) % Sz];
  return l.Search(v);
}

template <class T, class Fd, unsigned long Sz>
bool CHashMap<T, Fd, Sz>::rem(T v) {

  return bucket[fd(v) % Sz].Rem(v, 0);
}

template <class T, class Fd, unsigned long Sz>
void CHashMap<T, Fd, Sz>::PrintTable() const {
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