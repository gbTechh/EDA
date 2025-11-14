#include "vector.h"

template <class T> class ASC { // MIN-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a < b; }
};

template <class T> class DESC { // MAX-HEAP
public:
  bool operator()(const T &a, const T &b) const { return a > b; }
};

template <class T, class Compare = DESC<T>> class CHeap {
private:
  CVector<T> heap;
  Compare comp;

private:
  void swap(int i, int j);
  void sift_up(int idx);
  void sift_down(int idx);

public:
  CHeap() = default;

  int size() const;
  void push(T v);
  T top() const;
  bool pop();
  bool empty() const;
};

// Implementación

template <class T, class Compare>
void CHeap<T, Compare>::swap(int index1, int index2) {
  T temp = heap[index1];
  heap[index1] = heap[index2];
  heap[index2] = temp;
}

template <class T, class Compare> int CHeap<T, Compare>::size() const {
  return heap.size();
}

template <class T, class Compare> bool CHeap<T, Compare>::empty() const {
  return heap.size() == 0;
}

template <class T, class Compare> void CHeap<T, Compare>::sift_up(int idx) {
  while (idx > 0) {
    int parent_idx = (idx - 1) / 2;

    if (comp(heap[idx], heap[parent_idx])) { // functor
      swap(idx, parent_idx);
      idx = parent_idx;
    } else {
      break;
    }
  }
}

template <class T, class Compare> void CHeap<T, Compare>::sift_down(int idx) {
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

template <class T, class Compare> void CHeap<T, Compare>::push(T v) {
  heap.push_back(v);
  sift_up(heap.size() - 1);
}

template <class T, class Compare> T CHeap<T, Compare>::top() const {
  if (heap.size() == 0) {
    throw std::out_of_range("Heap vacío");
  }
  return heap[0];
}

template <class T, class Compare> bool CHeap<T, Compare>::pop() {
  if (heap.size() == 0) {
    return false;
  }

  heap[0] = heap[heap.size() - 1];
  heap.pop_back();

  if (heap.size() > 0) {
    sift_down(0);
  }

  return true;
}