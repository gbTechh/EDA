#include "../minheap.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

void test_cheap() {
  std::cout << "=== TESTING CHEAP (MAX-HEAP) ===" << std::endl;

  // Test 1: Heap vacío
  {
    CHeap<int> heap;
    assert(heap.size() == 0);
    assert(heap.empty() == true);
    std::cout << "✓ Test 1 pasado: heap vacío" << std::endl;
  }

  // Test 2: Inserción básica y top() - AHORA ES MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(5);
    assert(heap.size() == 1);
    assert(heap.empty() == false);
    assert(heap.top() == 5);

    heap.push(3);
    assert(heap.top() == 5); // 5 sigue siendo el máximo

    heap.push(8);
    assert(heap.top() == 8); // 8 es el nuevo máximo
    std::cout << "✓ Test 2 pasado: inserción básica y top()" << std::endl;
  }

  // Test 3: Múltiples inserciones y propiedad de heap - MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(10);
    heap.push(5);
    heap.push(15);
    heap.push(3);
    heap.push(7);
    heap.push(1);

    assert(heap.size() == 6);
    assert(heap.top() == 15); // El MÁXIMO debe ser 15
    std::cout << "✓ Test 3 pasado: múltiples inserciones" << std::endl;
  }

  // Test 4: pop() básico - MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(5);
    heap.push(3);
    heap.push(8);

    assert(heap.pop() == true);
    assert(heap.size() == 2);
    assert(heap.top() == 5); // Después de remover 8, el máximo es 5

    assert(heap.pop() == true);
    assert(heap.top() == 3);

    assert(heap.pop() == true);
    assert(heap.empty() == true);
    std::cout << "✓ Test 4 pasado: pop() básico" << std::endl;
  }

  // Test 5: pop() en heap vacío
  {
    CHeap<int> heap;
    assert(heap.pop() == false);
    std::cout << "✓ Test 5 pasado: pop() en heap vacío" << std::endl;
  }

  // Test 6: Secuencia de push y pop - MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(20);
    heap.push(10);
    heap.push(30);

    assert(heap.top() == 30); // Máximo
    heap.pop();

    assert(heap.top() == 20);
    heap.push(35);

    assert(heap.top() == 35); // Nuevo máximo
    heap.pop();

    assert(heap.top() == 20);
    heap.pop();

    assert(heap.top() == 10);
    heap.pop();

    assert(heap.empty() == true);
    std::cout << "✓ Test 6 pasado: secuencia push/pop" << std::endl;
  }

  // Test 7: Elementos duplicados - MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(5);
    heap.push(5);
    heap.push(3);
    heap.push(3);
    heap.push(7);

    assert(heap.top() == 7); // Máximo
    heap.pop();
    assert(heap.top() == 5); // Primer 5
    heap.pop();
    assert(heap.top() == 5); // Segundo 5
    heap.pop();
    assert(heap.top() == 3); // Primer 3
    std::cout << "✓ Test 7 pasado: elementos duplicados" << std::endl;
  }

  // Test 8: Ordenamiento implícito (heap sort básico) - MAX-HEAP
  {
    CHeap<int> heap;
    std::vector<int> input = {9, 2, 7, 1, 8, 3, 6, 4, 5};
    std::vector<int> output;

    for (int num : input) {
      heap.push(num);
    }

    while (!heap.empty()) {
      output.push_back(heap.top());
      heap.pop();
    }

    // Verificar que salen en orden DESCENDENTE (max-heap)
    assert(std::is_sorted(output.rbegin(), output.rend()));
    assert(output.size() == input.size());
    std::cout << "✓ Test 8 pasado: ordenamiento implícito DESCENDENTE"
              << std::endl;
  }

  // Test 9: Strings (test con otro tipo de dato) - MAX-HEAP
  {
    CHeap<std::string> heap;
    heap.push("zebra");
    heap.push("apple");
    heap.push("banana");

    assert(heap.top() == "zebra"); // Máximo alfabético
    heap.pop();
    assert(heap.top() == "banana");
    heap.pop();
    assert(heap.top() == "apple");
    std::cout << "✓ Test 9 pasado: strings" << std::endl;
  }

  // Test 10: Números negativos - MAX-HEAP
  {
    CHeap<int> heap;
    heap.push(5);
    heap.push(-3);
    heap.push(0);
    heap.push(-10);
    heap.push(2);

    assert(heap.top() == 5); // Máximo
    heap.pop();
    assert(heap.top() == 2);
    heap.pop();
    assert(heap.top() == 0);
    std::cout << "✓ Test 10 pasado: números negativos" << std::endl;
  }

  // Test 11: Redimensionamiento automático (muchos elementos) - MAX-HEAP
  {
    CHeap<int> heap;
    for (int i = 0; i <= 100; ++i) {
      heap.push(i);
    }

    assert(heap.size() == 101);

    // Verificar que extrae en orden DESCENDENTE
    for (int i = 100; i >= 0; --i) {
      assert(heap.top() == i);
      heap.pop();
    }

    assert(heap.empty() == true);
    std::cout << "✓ Test 11 pasado: redimensionamiento automático" << std::endl;
  }

  std::cout << "✅ TODOS LOS TESTS DEL CHEAP (MAX-HEAP) PASADOS!" << std::endl;
}

// Test específico para MIN-HEAP explícito
void test_min_heap() {
  std::cout << "\n=== TESTING MIN-HEAP EXPLÍCITO ===" << std::endl;

  CHeap<int, ASC<int>> min_heap;

  min_heap.push(10);
  min_heap.push(5);
  min_heap.push(15);
  min_heap.push(3);

  assert(min_heap.top() == 3); // Mínimo
  min_heap.pop();
  assert(min_heap.top() == 5);

  std::cout << "✅ MIN-HEAP EXPLÍCITO FUNCIONA!" << std::endl;
}

// Test específico para verificar la propiedad de heap - MAX-HEAP
void test_heap_property() {
  std::cout << "\n=== TESTING PROPIEDAD DE HEAP (MAX-HEAP) ===" << std::endl;

  CHeap<int> heap; // Max-heap por defecto

  heap.push(50);
  heap.push(25);
  heap.push(75);
  heap.push(10);
  heap.push(35);
  heap.push(60);
  heap.push(90);
  heap.push(5);
  heap.push(15);

  std::vector<int> sorted;
  while (!heap.empty()) {
    sorted.push_back(heap.top());
    heap.pop();
  }

  // Verificar que salieron en orden DESCENDENTE
  assert(std::is_sorted(sorted.rbegin(), sorted.rend()));
  std::cout << "Elementos extraídos en orden (descendente): ";
  for (int num : sorted) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  std::cout << "✅ PROPIEDAD DE MAX-HEAP VERIFICADA!" << std::endl;
}

int main() {
  try {
    test_cheap();         // Tests para max-heap (por defecto)
    test_min_heap();      // Test para min-heap explícito
    test_heap_property(); // Test de propiedad para max-heap
    std::cout << "\n🎉 ¡TODOS LOS TESTS PASARON EXITOSAMENTE!" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "❌ Error en test: " << e.what() << std::endl;
    return 1;
  }
}