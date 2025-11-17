#ifndef LISTAMAP_H
#define LISTAMAP_H

#include "data.h"
#include <cstddef>
#include <iostream>

template <class K, class V> class NodeListMap {
public:
  KeyValue<K, V> data;
  NodeListMap *next;

  NodeListMap(KeyValue<K, V> _data, NodeListMap *n = nullptr)
      : data(_data), next(n) {}
  ~NodeListMap() {}
};

template <class K, class V> class CListMap {
public:
  int size;
  NodeListMap<K, V> *root;

public:
  CListMap() : size(0), root(nullptr) {}
  ~CListMap() { limpiar_lista(); }

  const KeyValue<K, V> *get(const K &key) const {
    NodeListMap<K, V> *current = root;
    while (current != nullptr) {
      if (current->data.key == key) {
        return &(current->data); // Devuelve un puntero seguro
      }
      current = current->next;
    }
    return nullptr; // No encontrado
  }
  V &operator[](const K &key) {
    NodeListMap<K, V> *current = root;
    while (current != nullptr) {
      if (current->data.key == key) {
        return current->data.value;
      }
      current = current->next;
    }

    Ins(KeyValue<K, V>(key));
    return root->data.value;
  }

  bool Find(NodeListMap<K, V> *&n);
  bool Search(KeyValue<K, V> d);
  bool SearchByValue(const V &val) const;
  void Ins(KeyValue<K, V> data);
  bool Rem(K k);
  void PrintList() const;
  void limpiar_lista();
};

// Implementacion

template <class K, class V> bool CListMap<K, V>::Find(NodeListMap<K, V> *&n) {
  NodeListMap<K, V> *current = root;
  while (current != nullptr) {
    if (current->data == n->data) {
      n = current;
      return true;
    }
    current = current->next;
  }
  return false;
}

template <class K, class V> bool CListMap<K, V>::Search(KeyValue<K, V> d) {
  NodeListMap<K, V> *current = root;
  while (current != nullptr) {
    if (current->data == d) {
      return true;
    }
    current = current->next;
  }
  return false;
}

template <class K, class V>
bool CListMap<K, V>::SearchByValue(const V &val) const {
  NodeListMap<K, V> *current = root;

  while (current != nullptr) {
    if (current->data.value == val) {
      return true;
    }
    current = current->next;
  }

  return false;
}
template <class K, class V> void CListMap<K, V>::Ins(KeyValue<K, V> _data) {

  NodeListMap<K, V> *current = root;
  while (current != nullptr) {
    if (current->data.key == _data.key) {
      current->data.value = _data.value;
      return;
    }
    current = current->next;
  }

  NodeListMap<K, V> *newNode = new NodeListMap<K, V>(_data);

  if (root == nullptr) {
    root = newNode;
  } else {
    newNode->next = root;
    root = newNode;
  }
  size++;
}

template <class K, class V> bool CListMap<K, V>::Rem(K key) {
  if (root == nullptr)
    return false;

  NodeListMap<K, V> *current = root;
  NodeListMap<K, V> *previous = nullptr;

  while (current != nullptr) {
    if (current->data == key) {

      if (previous == nullptr) {
        root = current->next;
      } else {
        previous->next = current->next;
      }

      delete current;
      size--;
      return true;
    }
    previous = current;
    current = current->next;
  }
  return false;
}

template <class K, class V> void CListMap<K, V>::PrintList() const {
  NodeListMap<K, V> *current = root;
  if (current == nullptr) {
    std::cout << "(vacio)";
    return;
  }

  while (current != nullptr) {
    std::cout << "[" << current->data.key << "]"; // Para KeyValue
    // cout << "[" << current->data.topic << "]"; // Para Data
    if (current->next != nullptr)
      std::cout << " -> ";
    current = current->next;
  }
}

template <class K, class V> void CListMap<K, V>::limpiar_lista() {
  NodeListMap<K, V> *current = root;
  while (current != nullptr) {
    NodeListMap<K, V> *next = current->next;
    delete current;
    current = next;
  }
  root = nullptr;
  size = 0;
}
#endif