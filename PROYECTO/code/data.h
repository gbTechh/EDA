// data.h
#ifndef DATA_H
#define DATA_H

#include <string>

struct Data {
  std::string topic;
  int frq;
  int error;

  Data() : topic(""), frq(0), error(0) {}
  Data(const std::string &t, int f = 1) : topic(t), frq(f), error(0) {}
  Data(const std::string &t, int f, int e) : topic(t), frq(f), error(e) {}

  // Operador de comparación para búsquedas
  bool operator==(const Data &other) const { return topic == other.topic; }

  // Operador de asignación
  Data &operator=(const Data &other) {
    if (this != &other) {
      topic = other.topic;
      frq = other.frq;
      error = other.error;
    }
    return *this;
  }
};

#endif