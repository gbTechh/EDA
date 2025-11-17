#include "data.h"
#include <iostream>

class Node {
public:
  Data data;

public:
  Node *next;
  Node() = default;
  Node(Data data, Node *n = nullptr);

  ~Node();
};

class CList {

public:
  int size;  // total de nodos
  int total; // total de elementos incluidas las frecuencias
  Node *root;

public:
  CList();
  ~CList();

  bool Find(Node *&n);
  bool Search(Data &d);
  void Ins(Data &data);
  bool Rem(Data &data);
  void PrintList() const;
  void limpiar_lista();
};
