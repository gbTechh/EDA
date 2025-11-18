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
private:
  // Contador de debug
  long long total_search_steps = 0;
  long long total_operations = 0;

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
  bool RemFreq(Data &data);
  void PrintList() const;
  void limpiar_lista();
  void printDebugStats();
};
