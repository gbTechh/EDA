#include "data.h"
#include "minheapindice.h"
#include <cstdio>
#include <iostream>

using namespace std;

int main() {

  CMinHeapInd<Data> heap;
  int i = -2;

  i = heap.push({"arroz", 4, 0, 0});
  int inx = heap.get_position("arroz");
  cout << "index: " << i << "\n";
  heap.print();

  i = heap.push({"papa", 1, 0, 0});
  cout << "index: " << i << "\n";
  heap.print();

  i = heap.push({"camote", 2, 0, 0});
  cout << "index: " << i << "\n";
  heap.print();

  i = heap.push({"pollo", 2, 0, 0});
  cout << "index: " << i << "\n";
  heap.print();

  i = heap.push({"fideo", 0, 0, 0});
  cout << "index: " << i << "\n";

  heap.print();
  heap.update("papa", 2);

  cout << "POSICION: " << heap.get_position("papa") << "\n";
  heap.print();
}