#include "../data.h" // Asegúrate de incluir tu archivo data.h corregido con operator< y operator>
#include "../minheap.h" // Asegúrate de incluir tu minheap.h con las guardas de inclusión
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

void test_min_heap_data() {
  std::cout << "=== TEST MIN-HEAP con tipo Data ===" << std::endl;

  // CHeap<Data, ASC<Data>> es equivalente a CHeap<Data> (por el default
  // template)
  CHeap<Data> min_heap;

  // Insertamos datos con frecuencias:
  min_heap.push(Data("futbol", 5));
  min_heap.push(Data("tenis", 2));
  min_heap.push(Data("golf", 10));
  min_heap.push(Data("baloncesto", 1)); // Este es el mínimo

  assert(min_heap.size() == 4);
  std::cout << "✓ Inserción de 4 elementos exitosa." << std::endl;

  // El top en un MIN-HEAP debe ser el elemento con la FRECUENCIA MÁS BAJA (1)
  Data top_element = min_heap.top();
  assert(top_element.topic == "baloncesto");
  assert(top_element.frq == 1);
  std::cout << "✓ Top es el mínimo esperado ('baloncesto' con frq 1)."
            << std::endl;

  // Hacemos pop y verificamos el nuevo mínimo
  min_heap.pop();
  assert(min_heap.size() == 3);

  top_element = min_heap.top();
  // El siguiente más bajo es 'tenis' con frq 2
  assert(top_element.topic == "tenis");
  assert(top_element.frq == 2);
  std::cout
      << "✓ Pop funciona, nuevo top es el siguiente mínimo ('tenis' con frq 2)."
      << std::endl;

  // Probamos con un heap vacío
  CHeap<Data> empty_heap;
  assert(empty_heap.empty() == true);
  assert(empty_heap.pop() == false); // Pop en vacío devuelve false

  try {
    empty_heap.top(); // Intentar top en vacío debe lanzar excepción
    assert(false && "Top en heap vacío debió lanzar excepción.");
  } catch (const std::out_of_range &e) {
    std::cout << "✓ Manejo correcto de heap vacío (pop/top)." << std::endl;
  }

  std::cout << "✅ TEST MIN-HEAP PASADO" << std::endl;
}

int main() {
  test_min_heap_data();
  std::cout
      << "\nTodos los tests de CHeap (Min-Heap) pasaron satisfactoriamente."
      << std::endl;
  return 0;
}
