#include "map.h"
#include "vector.h"
#include <iostream>

template <class T> class ASC { // MIN-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a < b; }
};

template <class T> class DESC { // MAX-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a > b; }
};

struct Indices {
  int indexViejo;
  int indexNuevo;
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
template <class T, unsigned long BucketsizeMap = 1999, class Compare = ASC<T>>
class CMinHeapInd {
private:
  CVector<T> heap;
  CMap<std::string, int, StringHash, BucketsizeMap> posiciones;
  Compare comp;

private:
  void swap(int i, int j) {
    // Intercambiar elementos en el heap
    T temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;

    // Actualizar posiciones en el índice
    posiciones.rem(heap[i].topic);
    posiciones.ins(heap[i].topic, i);

    posiciones.rem(heap[j].topic);
    posiciones.ins(heap[j].topic, j);
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
  CMinHeapInd() = default;
  const CVector<T> &get_heap_data() const { return heap; }
  bool exists(const std::string &palabra) { return posiciones.findk(palabra); }

  int get_position(const std::string &palabra) {
    try {
      return posiciones.find(palabra);
    } catch (const std::out_of_range &) {
      return -1;
    }
  }

  int update(const std::string &palabra, int nueva_frecuencia) {
    if (!exists(palabra))
      return -1;

    int pos = get_position(palabra);
    if (pos == -1)
      return -1;

    int vieja_frecuencia = heap[pos].frq;

    if (nueva_frecuencia == vieja_frecuencia)
      return pos;

    heap[pos].frq = nueva_frecuencia;

    if (nueva_frecuencia < vieja_frecuencia) {
      sift_up(pos);
    } else {
      sift_down(pos);
    }

    return get_position(palabra);
  }

  int push(T v) {
    if (exists(v.topic)) {
      return update(v.topic, v.frq);
    }

    heap.push_back(v);
    posiciones.ins(v.topic, heap.size() - 1);
    sift_up(heap.size() - 1);

    return get_position(v.topic);
  }

  bool pop() {
    if (heap.empty())
      return false;

    posiciones.rem(heap[0].topic);

    if (heap.size() == 1) {
      heap.pop_back();
      return true;
    }

    heap[0] = heap[heap.size() - 1];
    heap.pop_back();

    posiciones.ins(heap[0].topic, 0);
    sift_down(0);

    return true;
  }

  bool remove(const std::string &palabra) {
    if (!exists(palabra))
      return false;

    int pos = get_position(palabra);
    posiciones.rem(palabra);

    if (static_cast<std::size_t>(pos) == heap.size() - 1) {
      heap.pop_back();
      return true;
    }

    heap[pos] = heap[heap.size() - 1];
    heap.pop_back();

    posiciones.ins(heap[pos].topic, pos);

    // Reordenar
    if (pos > 0 && comp(heap[pos], heap[(pos - 1) / 2])) {
      sift_up(pos);
    } else {
      sift_down(pos);
    }

    return true;
  }

  std::size_t size() const { return heap.size(); }
  bool empty() const { return heap.empty(); }

  T top() const {
    if (heap.empty())
      throw std::out_of_range("Heap vacío");
    return heap[0];
  }

  int get_min_frequency() const { return empty() ? 0 : heap[0].frq; }

  void print() const {
    if (heap.empty()) {
      std::cout << "(Heap vacío)" << std::endl;
      return;
    }
    std::cout << "Contenido del Heap: ";
    for (std::size_t i = 0; i < heap.size(); ++i) {
      std::cout << heap[i];
      if (i < heap.size() - 1)
        std::cout << ", ";
    }
    std::cout << std::endl;
  }
};