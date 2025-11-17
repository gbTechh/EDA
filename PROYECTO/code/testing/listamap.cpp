#include "../listamap.h" // Asegúrate de tener la ruta correcta
#include "../data.h"     // Asumimos KeyValue K->V está aquí
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

// --- Funciones de Testing Existentes (Ligeramente modificadas para usar el
// nuevo []) ---

void test_mapa_con_operador_modificable() {
  std::cout << "=== TEST LISTAMAP MODIFICABLE (string, int) ===" << std::endl;
  CListMap<std::string, int> mapa; // K=string, V=int

  // Test 1: Insertar elementos usando operador []
  mapa["frutas"] = 10; // Uso estándar: mapa[clave] = valor;
  mapa["vegetales"] = 20;

  assert(mapa.size == 2);
  assert(mapa.root != nullptr);
  assert(mapa.root->data.key == "vegetales"); // inserta al inicio
  assert(mapa["frutas"] == 10);
  std::cout << "✓ Inserción y modificación con []" << std::endl;

  // Test 2: Acceder a un elemento existente con [] (lectura/escritura)
  mapa["frutas"] = 15; // Actualiza el valor

  assert(mapa.size == 2);
  assert(mapa["frutas"] == 15);
  std::cout << "✓ Acceso y actualización con []" << std::endl;

  std::cout << "✅ TEST LISTAMAP MODIFICABLE PASADO" << std::endl;
}

void test_const_access() {
  std::cout << "\n=== TEST ACCESO CONST (int, string) ===" << std::endl;
  CListMap<int, std::string> mapa_int;
  mapa_int[1] = "uno"; // Usa operator[] no-const para insertar
  mapa_int[2] = "dos";

  const CListMap<int, std::string> &mapa_const = mapa_int;

  // Lectura normal usando get()
  const KeyValue<int, std::string> *kv_uno = mapa_const.get(1);
  assert(kv_uno != nullptr);
  assert(kv_uno->value == "uno");
  std::cout << "✓ Acceso const a elemento existente funciona con get()"
            << std::endl;

  // Verificar el manejo de clave no existente con get()
  const KeyValue<int, std::string> *kv_noventa = mapa_const.get(99);
  assert(kv_noventa == nullptr); // Esperamos nullptr
  std::cout << "✓ get() devuelve nullptr para elemento no existente"
            << std::endl;

  std::cout << "✅ TEST ACCESO CONST PASADO" << std::endl;
}

// --- Nuevas Funciones de Testing para REM y SearchByValue ---

void test_remocion_elementos() {
  std::cout << "\n=== TEST REMOCION DE ELEMENTOS (REM) ===" << std::endl;
  CListMap<int, std::string> mapa_rem;

  mapa_rem[1] = "Apple";
  mapa_rem[2] = "Banana";
  mapa_rem[3] = "Cherry";
  assert(mapa_rem.size == 3);

  // Test 1: Remover un elemento intermedio (Banana, key 2)
  bool removed_banana = mapa_rem.Rem(2);
  assert(removed_banana == true);
  assert(mapa_rem.size == 2);
  assert(mapa_rem.get(2) == nullptr);
  assert(mapa_rem.get(1) != nullptr);
  assert(mapa_rem.get(3) != nullptr);
  std::cout << "✓ Remoción de elemento intermedio (Key 2) exitosa" << std::endl;

  // Test 2: Remover el elemento raíz/primero (Cherry, key 3 porque inserta al
  // inicio) El orden de inserción fue 3, 2, 1. La raíz actual es 3.
  bool removed_cherry = mapa_rem.Rem(3);
  assert(removed_cherry == true);
  assert(mapa_rem.size == 1);
  assert(mapa_rem.get(3) == nullptr);
  assert(mapa_rem.get(1) != nullptr);
  std::cout << "✓ Remoción del elemento raíz (Key 3) exitosa" << std::endl;

  // Test 3: Remover el último elemento restante (Apple, key 1)
  bool removed_apple = mapa_rem.Rem(1);
  assert(removed_apple == true);
  assert(mapa_rem.size == 0);
  assert(mapa_rem.root == nullptr);
  std::cout << "✓ Remoción del último elemento exitosa" << std::endl;

  // Test 4: Intentar remover de una lista vacía
  bool removed_empty = mapa_rem.Rem(99);
  assert(removed_empty == false);
  std::cout << "✓ Manejo de remoción en lista vacía" << std::endl;

  // Test 5: Intentar remover una clave que no existe
  mapa_rem[10] = "Diez";
  bool removed_nonexistent = mapa_rem.Rem(999);
  assert(removed_nonexistent == false);
  assert(mapa_rem.size == 1); // El tamaño no debe cambiar
  std::cout << "✓ Manejo de remoción de clave inexistente" << std::endl;

  std::cout << "✅ TEST REMOCION PASADO" << std::endl;
}

void test_search_by_value() {
  std::cout << "\n=== TEST SEARCH BY VALUE (SearchByValue) ===" << std::endl;
  CListMap<int, std::string> mapa_val;

  mapa_val[1] = "Rojo";
  mapa_val[2] = "Verde";
  mapa_val[3] = "Azul";

  // Test 1: Buscar un valor existente
  assert(mapa_val.SearchByValue("Verde") == true);
  std::cout << "✓ Encontrado valor existente 'Verde'" << std::endl;

  // Test 2: Buscar otro valor existente
  assert(mapa_val.SearchByValue("Rojo") == true);
  std::cout << "✓ Encontrado valor existente 'Rojo'" << std::endl;

  // Test 3: Buscar un valor que no existe
  assert(mapa_val.SearchByValue("Amarillo") == false);
  std::cout << "✓ Valor inexistente 'Amarillo' manejado correctamente"
            << std::endl;

  // Test 4: Buscar en una lista vacía
  CListMap<int, std::string> empty_map;
  assert(empty_map.SearchByValue("CualquierCosa") == false);
  std::cout << "✓ Manejo de búsqueda en lista vacía" << std::endl;

  std::cout << "✅ TEST SEARCH BY VALUE PASADO" << std::endl;
}

int main() {
  // Ejecutar todos los tests
  test_mapa_con_operador_modificable();
  test_const_access();
  test_remocion_elementos();
  test_search_by_value();

  std::cout << "\nTodos los tests de CListMap han pasado satisfactoriamente."
            << std::endl;

  return 0;
}
