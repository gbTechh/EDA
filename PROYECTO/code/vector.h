#include <iostream>
#include <stdexcept>

template <class T> class CVector {
private:
  T *arr = nullptr;
  std::size_t capacity = 0;
  std::size_t len = 0;
  void resize();

public:
  CVector(std::size_t x);
  CVector();

  ~CVector();
  CVector(const CVector &other);
  CVector &operator=(const CVector &other);

  std::size_t size() const { return len; }
  T &operator[](std::size_t ind);
  const T &operator[](std::size_t ind) const;

  void push_front(T v);
  void push_back(T v);
  void pop_front();
  void pop_back();
};

// IMPLEMENTACION

template <class T> CVector<T>::CVector(std::size_t x) : len(x) {
  capacity = (x == 0) ? 1 : x;
  arr = new T[capacity];
}

template <class T> CVector<T>::CVector() : capacity(4), len(0) {
  arr = new T[capacity];
}

template <class T> CVector<T>::~CVector() { delete[] arr; }

template <class T>
CVector<T>::CVector(const CVector &other)
    : capacity(other.capacity), len(other.len) {
  arr = new T[capacity];
  for (std::size_t i = 0; i < len; ++i) {
    arr[i] = other.arr[i];
  }
}

template <class T> CVector<T> &CVector<T>::operator=(const CVector &other) {
  if (this != &other) {

    delete[] arr;

    capacity = other.capacity;
    len = other.len;
    arr = new T[capacity];

    for (std::size_t i = 0; i < len; ++i) {
      arr[i] = other.arr[i];
    }
  }
  return *this;
}

template <class T> void CVector<T>::resize() {
  capacity = capacity == 0 ? 4 : capacity * 2;
  T *newArr = new T[capacity];

  for (std::size_t i = 0; i < len; i++) {
    newArr[i] = arr[i];
  }

  delete[] arr;
  arr = newArr;
}

template <class T> T &CVector<T>::operator[](std::size_t ind) {
  if (ind >= len) {
    throw std::out_of_range("INDEX FUERA DEL RANGO");
  }
  return arr[ind];
}

template <class T> const T &CVector<T>::operator[](std::size_t ind) const {
  if (ind >= len) {
    throw std::out_of_range("INDEX FUERA DEL RANGO");
  }
  return arr[ind];
}

template <class T> void CVector<T>::push_front(T v) {
  if (len >= capacity) {
    resize();
  }

  for (std::size_t i = len; i > 0; --i) {
    arr[i] = arr[i - 1];
  }

  arr[0] = v;
  len++;
}

template <class T> void CVector<T>::push_back(T v) {
  if (len >= capacity) {
    resize();
  }
  arr[len] = v;
  len++;
}

template <class T> void CVector<T>::pop_front() {
  if (len == 0)
    return;

  for (std::size_t i = 0; i < len - 1; i++) {
    arr[i] = arr[i + 1];
  }

  len--;
}

template <class T> void CVector<T>::pop_back() {
  if (len == 0)
    return;
  len--;
}
