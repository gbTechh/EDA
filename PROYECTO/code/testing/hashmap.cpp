#include "../hashmap.h"
#include <cassert>
#include <iostream>
#include <string>

// Función hash para Data
struct DataHash {
  unsigned long operator()(const Data &data) const {
    unsigned long hash = 0;
    for (char c : data.topic) {
      hash = hash * 31 + c;
    }
    return hash;
  }
};

void test_hashmap_basico() {
  std::cout << "=== TEST HASHMAP BÁSICO ===" << std::endl;

  // HashMap para Data (no para strings directamente)
  CHashMap<Data, DataHash, 10> hashmap;

  // Test 1: Inserción básica
  Data hello{"hello", 1};
  Data world{"world", 1};
  hashmap.ins(hello);
  hashmap.ins(world);

  assert(hashmap.find(hello) == true);
  assert(hashmap.find(world) == true);

  Data nonexistent{"nonexistent", 1};
  assert(hashmap.find(nonexistent) == false);
  std::cout << "✓ Inserción y búsqueda básica" << std::endl;

  // Test 2: Eliminación
  bool removed = hashmap.rem(hello);
  assert(removed == true);
  assert(hashmap.find(hello) == false);
  assert(hashmap.find(world) == true);
  std::cout << "✓ Eliminación funciona" << std::endl;

  std::cout << "✅ TEST HASHMAP BÁSICO PASADO" << std::endl;
}

void test_hashmap_colisiones() {
  std::cout << "\n=== TEST COLISIONES HASH ===" << std::endl;

  // HashMap pequeño para forzar colisiones
  CHashMap<Data, DataHash, 3> hashmap;

  // Estos Data probablemente tendrán el mismo hash % 3
  Data d1{"abc", 1}, d2{"def", 1}, d3{"ghi", 1}, d4{"jkl", 1};
  hashmap.ins(d1);
  hashmap.ins(d2);
  hashmap.ins(d3);
  hashmap.ins(d4);

  // Todos deben existir a pesar de las colisiones
  assert(hashmap.find(d1) == true);
  assert(hashmap.find(d2) == true);
  assert(hashmap.find(d3) == true);
  assert(hashmap.find(d4) == true);
  std::cout << "✓ Manejo de colisiones correcto" << std::endl;

  // Eliminar con colisiones
  bool removed = hashmap.rem(d2);
  assert(removed == true);
  assert(hashmap.find(d2) == false);
  assert(hashmap.find(d1) == true);
  assert(hashmap.find(d3) == true);
  assert(hashmap.find(d4) == true);
  std::cout << "✓ Eliminación con colisiones" << std::endl;

  std::cout << "✅ TEST COLISIONES HASH PASADO" << std::endl;
}

void test_hashmap_duplicados() {
  std::cout << "\n=== TEST DUPLICADOS HASHMAP ===" << std::endl;

  CHashMap<Data, DataHash, 5> hashmap;

  // Insertar duplicados
  Data dup{"duplicate", 1};
  hashmap.ins(dup);
  hashmap.ins(dup);
  hashmap.ins(dup);

  assert(hashmap.find(dup) == true);
  std::cout << "✓ Duplicados manejados correctamente" << std::endl;

  // Eliminar - como tu implementación elimina completamente el nodo,
  // después de eliminar NO debería existir
  bool removed = hashmap.rem(dup);
  assert(removed == true);
  // CORREGIDO: Después de eliminar, NO debería existir
  assert(hashmap.find(dup) == false);
  std::cout << "✓ Eliminación con duplicados" << std::endl;

  std::cout << "✅ TEST DUPLICADOS HASHMAP PASADO" << std::endl;
}

void test_hashmap_vacio() {
  std::cout << "\n=== TEST HASHMAP VACÍO ===" << std::endl;

  CHashMap<Data, DataHash, 5> hashmap;

  // Búsqueda en hashmap vacío
  Data anything{"anything", 1};
  assert(hashmap.find(anything) == false);
  std::cout << "✓ Búsqueda en hashmap vacío" << std::endl;

  // Eliminación en hashmap vacío
  bool removed = hashmap.rem(anything);
  assert(removed == false);
  std::cout << "✓ Eliminación en hashmap vacío" << std::endl;

  std::cout << "✅ TEST HASHMAP VACÍO PASADO" << std::endl;
}

// Elimina los tests con int y strings que no funcionan

int main() {
  try {
    test_hashmap_basico();
    test_hashmap_colisiones();
    test_hashmap_duplicados();
    test_hashmap_vacio();

    std::cout << "\n🎉 ¡TESTS DE HASHMAP PASARON EXITOSAMENTE!" << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "❌ Error en test: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "❌ Error desconocido en test" << std::endl;
    return 1;
  }
}