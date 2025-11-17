#include "lista.h"
#include <iostream>

template <class T, class S, class Fd, unsigned long Sz> class CHashMap {
private:
  int sz = Sz;

public:
  S bucket[Sz];
  Fd fd;
  int num_podas = 0;

public:
  CHashMap() = default;
  // ~CHashMap();

  void ins(T &v);
  bool find(T v);
  bool search(T &v);
  bool remFreq(T &v);
  bool rem(T v);
  int size();
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

template <class T, class S, class Fd, unsigned long Sz>
void CHashMap<T, S, Fd, Sz>::ins(T &v) {
  bucket[fd(v) % Sz].Ins(v);
}

template <class T, class S, class Fd, unsigned long Sz>
int CHashMap<T, S, Fd, Sz>::size() {
  return sz;
}

template <class T, class S, class Fd, unsigned long Sz>
bool CHashMap<T, S, Fd, Sz>::find(T v) {
  CList &l = bucket[fd(v) % Sz];
  return l.Search(v);
}
template <class T, class S, class Fd, unsigned long Sz>
bool CHashMap<T, S, Fd, Sz>::search(T &v) {
  CList &l = bucket[fd(v) % Sz];
  return l.Search(v);
}

template <class T, class S, class Fd, unsigned long Sz>
bool CHashMap<T, S, Fd, Sz>::remFreq(T &v) {
  return bucket[fd(v) % Sz].RemFreq(&v);
}

template <class T, class S, class Fd, unsigned long Sz>
bool CHashMap<T, S, Fd, Sz>::rem(T v) {

  return bucket[fd(v) % Sz].Rem(v);
}

template <class T, class S, class Fd, unsigned long Sz>
void CHashMap<T, S, Fd, Sz>::PrintTable() const {
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