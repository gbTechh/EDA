// data.h
#ifndef DATA_H
#define DATA_H

#include "vector.h"
#include <string>

template <class K, class V> struct KeyValue {
  K key;
  V value;
  KeyValue() : key(K{}), value(V{}) {}
  KeyValue(K k, V v = {}) : key(k), value(v) {}

  bool operator==(const KeyValue &other) const { return key == other.key; }

  bool operator==(const K &k) const { return key == k; }
};

struct Data {
  std::string topic;
  int frq;
  int error;
  int ultima_ventana;

  Data() : topic(""), frq(0), error(0), ultima_ventana(0) {}
  Data(const std::string &t, int f = 1) : topic(t), frq(f), error(0) {}
  Data(const std::string &t, int f, int e, int uv = 0)
      : topic(t), frq(f), error(e), ultima_ventana(uv) {}

  Data(const Data &other) = default;

  bool operator==(const Data &other) const { return topic == other.topic; }

  Data &operator=(const Data &other) {
    if (this != &other) {
      topic = other.topic;
      frq = other.frq;
      error = other.error;
    }
    return *this;
  }

  bool operator<(const Data &other) const { return frq < other.frq; }

  bool operator>(const Data &other) const { return frq > other.frq; }

  friend std::ostream &operator<<(std::ostream &os, const Data &d) {
    os << "[" << d.topic << " (frq:" << d.frq << ")]";
    return os;
  }
};

#endif