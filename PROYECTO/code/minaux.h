#ifndef MINAUX_H
#define MINAUX_H
#include "map.h"
#include "vector.h"
#include <string>

template <class T> class ASC { // MIN-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a < b; }
};

template <class T> class DESC { // MAX-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a > b; }
};

struct StringHash {
  inline unsigned long operator()(const std::string &x) const {
    unsigned long hash_value = 0;
    int i = 1;
    for (char c : x) {
      hash_value += static_cast<unsigned long>(c) * i + i;
      i++;
    }
    return hash_value;
  }
};

template <class T, class Compare = ASC<T>> class CHeap {
private:
  CVector<T> heap;
  CMap<std::string, int, StringHash, 101> posiciones;
  Compare comp;

private:
  void swap(int i, int j) {
    if (posiciones.findk(heap[i].topic)) {
      posiciones.rem(heap[i].topic);
    }
    if (posiciones.findk(heap[j].topic)) {
      posiciones.rem(heap[j].topic);
    }

    posiciones.ins(heap[i].topic, j);
    posiciones.ins(heap[j].topic, i);

    T temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
  }

  void sift_up(int idx) {
    while (idx > 0) {
      int parent_idx = (idx - 1) / 2;
      if (comp(heap[idx], heap[parent_idx])) {
        swap(idx, parent_idx);
        idx = parent_idx;
      } else {
        break;
      }
    }
  }

  void sift_down(int idx) {
    int size = heap.size();
    while (2 * idx + 1 < size) {
      int left_child = 2 * idx + 1;
      int right_child = 2 * idx + 2;
      int extreme = left_child;

      if (right_child < size && comp(heap[right_child], heap[left_child])) {
        extreme = right_child;
      }

      if (comp(heap[extreme], heap[idx])) {
        swap(idx, extreme);
        idx = extreme;
      } else {
        break;
      }
    }
  }

public:
  CHeap() = default;

  bool exists(const std::string &palabra) { return posiciones.findk(palabra); }

  int get_position(const std::string &palabra) {
    try {
      return posiciones.find(palabra);
    } catch (const std::out_of_range &) {
      return -1;
    }
  }

  void update(const std::string &palabra, int nueva_frecuencia) {
    if (!exists(palabra))
      return;

    int pos = get_position(palabra);
    if (pos == -1)
      return;

    int vieja_frecuencia = heap[pos].frq;
    heap[pos].frq = nueva_frecuencia;

    if (nueva_frecuencia > vieja_frecuencia) {
      sift_up(pos);
    } else {
      sift_down(pos);
    }
  }

  void push(T v) {
    if (exists(v.topic)) {
      update(v.topic, v.frq);
    } else {
      heap.push_back(v);
      posiciones.ins(v.topic, heap.size() - 1);
      sift_up(heap.size() - 1);
    }
  }

  bool pop() {
    if (heap.size() == 0)
      return false;

    if (posiciones.findk(heap[0].topic)) {
      posiciones.rem(heap[0].topic);
    }

    if (heap.size() > 1) {
      heap[0] = heap[heap.size() - 1];

      if (posiciones.findk(heap[0].topic)) {
        posiciones.rem(heap[0].topic);
      }
      posiciones.ins(heap[0].topic, 0);
    }

    heap.pop_back();

    if (heap.size() > 0) {
      sift_down(0);
    }

    return true;
  }

  int size() const { return heap.size(); }

  T top() const {
    if (heap.size() == 0)
      throw std::out_of_range("Heap vacío");
    return heap[0];
  }

  bool empty() const { return heap.size() == 0; }

  int get_min_frequency() const { return empty() ? 0 : heap[0].frq; }

  void print() const {
    if (heap.size() == 0) {
      std::cout << "(Heap vacío)" << std::endl;
      return;
    }
    std::cout << "Contenido del Heap: ";
    for (int i = 0; i < heap.size(); ++i) {
      std::cout << heap[i];
      if (i < heap.size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;

    // std::cout << "Posiciones: ";
    // posiciones.PrintTable();
  }
};

#endif