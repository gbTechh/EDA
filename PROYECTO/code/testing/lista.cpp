#include "../lista.h"
#include <cassert>
#include <iostream>
#include <string>

void test_lista_basica() {
  std::cout << "=== TEST LISTA BÁSICA ===" << std::endl;

  CList lista;

  // Test 1: Lista vacía
  assert(lista.size == 0);
  assert(lista.root == nullptr);
  std::cout << "✓ Lista vacía correcta" << std::endl;

  // Test 2: Insertar primer elemento
  Data d1{"topic1", 1};
  lista.Ins(d1);
  assert(lista.size == 1);
  assert(lista.total == 1);
  assert(lista.root != nullptr);
  assert(lista.root->data.topic == "topic1");
  assert(lista.root->data.frq == 1);
  std::cout << "✓ Inserción primer elemento" << std::endl;

  // Test 3: Insertar segundo elemento
  Data d2{"topic2", 1};
  lista.Ins(d2);
  assert(lista.size == 2);
  assert(lista.total == 2);
  assert(lista.root->data.topic == "topic2"); // Se inserta al inicio
  assert(lista.root->next->data.topic == "topic1");
  std::cout << "✓ Inserción segundo elemento" << std::endl;

  // Test 4: Búsqueda de elementos
  Data search{"topic1", 1};
  assert(lista.Search(search) == true);
  Data search2{"topic3", 1};
  assert(lista.Search(search2) == false);
  std::cout << "✓ Búsqueda funciona" << std::endl;

  std::cout << "✅ TEST LISTA BÁSICA PASADO" << std::endl;
}

void test_insercion_duplicados() {
  std::cout << "\n=== TEST INSERCIÓN DUPLICADOS ===" << std::endl;

  CList lista;

  // Insertar mismo topic múltiples veces
  Data d1{"topic1", 1};
  lista.Ins(d1);
  lista.Ins(d1); // Duplicado
  lista.Ins(d1); // Duplicado

  // El tamaño debe ser 1 (solo un nodo único)
  assert(lista.size == 1);
  assert(lista.total == 3);          // Pero total cuenta todas las inserciones
  assert(lista.root->data.frq == 3); // Frecuencia incrementada
  assert(lista.root->data.topic == "topic1");
  std::cout << "✓ Duplicados incrementan frecuencia" << std::endl;

  // Insertar topic diferente
  Data d2{"topic2", 1};
  lista.Ins(d2);
  assert(lista.size == 2);
  assert(lista.total == 4);
  std::cout << "✓ Inserción topic diferente funciona" << std::endl;

  std::cout << "✅ TEST INSERCIÓN DUPLICADOS PASADO" << std::endl;
}

void test_eliminacion() {
  std::cout << "\n=== TEST ELIMINACIÓN ===" << std::endl;

  CList lista;

  // Insertar varios elementos (recordar que se insertan al INICIO)
  Data d1{"topic1", 1}, d2{"topic2", 1}, d3{"topic3", 1};
  lista.Ins(d1); // Lista: topic1
  lista.Ins(d2); // Lista: topic2 -> topic1
  lista.Ins(d3); // Lista: topic3 -> topic2 -> topic1

  assert(lista.size == 3);

  // Test 1: Eliminar elemento del medio (topic2)
  {
    Data toRemove{"topic2", 1};
    bool removed = lista.Rem(toRemove);
    assert(removed == true);
    assert(toRemove.topic == "topic2");
    assert(lista.size == 2);
    // Lista ahora: topic3 -> topic1
    std::cout << "✓ Eliminación elemento del medio" << std::endl;
  }

  // Test 2: Eliminar primer elemento (topic3)
  {
    Data toRemove{"topic3", 1};
    bool removed = lista.Rem(toRemove);
    assert(removed == true);
    assert(toRemove.topic == "topic3");
    assert(lista.size == 1);
    assert(lista.root->data.topic == "topic1"); // Ahora topic1 es la raíz
    std::cout << "✓ Eliminación primer elemento" << std::endl;
  }

  // Test 3: Eliminar último elemento (topic1)
  {
    Data toRemove{"topic1", 1};
    bool removed = lista.Rem(toRemove);
    assert(removed == true);
    assert(toRemove.topic == "topic1");
    assert(lista.size == 0);
    assert(lista.root == nullptr);
    std::cout << "✓ Eliminación último elemento" << std::endl;
  }

  // Test 4: Eliminar de lista vacía
  {
    Data toRemove{"topic99", 1};
    bool removed = lista.Rem(toRemove);
    assert(removed == false);
    std::cout << "✓ Eliminación lista vacía" << std::endl;
  }

  // Test 5: Eliminar elemento que no existe
  {
    // Primero llenamos la lista
    Data d4{"topic4", 1}, d5{"topic5", 1};
    lista.Ins(d4); // topic4
    lista.Ins(d5); // topic5 -> topic4

    Data toRemove{"topic99", 1}; // No existe
    bool removed = lista.Rem(toRemove);
    assert(removed == false);
    assert(lista.size == 2); // Tamaño no cambia
    std::cout << "✓ Eliminación elemento no existente" << std::endl;
  }

  std::cout << "✅ TEST ELIMINACIÓN PASADO" << std::endl;
}
void debug_lista(const CList &lista, const std::string &mensaje) {
  std::cout << "DEBUG " << mensaje << ": ";
  lista.PrintList();
  std::cout << " | size: " << lista.size << " | total: " << lista.total
            << std::endl;
}
void test_eliminacion_simple() {
  std::cout << "\n=== TEST ELIMINACIÓN SIMPLE ===" << std::endl;

  CList lista;

  // Solo un elemento
  Data d1{"topic1", 1};
  lista.Ins(d1);
  debug_lista(lista, "Lista con un elemento");

  // Intentar eliminar ese elemento
  Data toRemove{"topic1", 1};
  Data removedData;
  bool removed = lista.Rem(removedData);

  debug_lista(lista, "Después de eliminar");
  std::cout << "Resultado eliminación: " << removed << std::endl;
  if (removed) {
    std::cout << "Dato removido: " << removedData.topic << std::endl;
  }

  assert(removed == true);
  assert(lista.size == 0);
  assert(lista.root == nullptr);
  std::cout << "✓ Eliminación simple funciona" << std::endl;
}

void test_eliminacion_debug() {
  std::cout << "\n=== TEST ELIMINACIÓN DEBUG ===" << std::endl;

  CList lista;

  // Insertar un elemento
  Data d1{"topic1", 1};
  lista.Ins(d1);
  std::cout << "Insertado: topic1" << std::endl;
  debug_lista(lista, "Después de insertar");

  // Ahora intentar eliminar
  Data toRemove{"topic1", 1};
  Data removedData;
  std::cout << "Intentando eliminar: '" << toRemove.topic << "'" << std::endl;
  bool removed = lista.Rem(toRemove);

  std::cout << "Resultado Rem: " << removed << std::endl;
  debug_lista(lista, "Después de Rem");

  // VERIFICACIÓN MANUAL
  if (!removed) {
    std::cerr << "❌ ERROR: Rem debería retornar true pero retornó false"
              << std::endl;
    exit(1);
  }
  if (lista.size != 0) {
    std::cerr << "❌ ERROR: Size debería ser 0 pero es " << lista.size
              << std::endl;
    exit(1);
  }
  if (lista.root != nullptr) {
    std::cerr << "❌ ERROR: Root debería ser nullptr" << std::endl;
    exit(1);
  }

  std::cout << "✓ Eliminación exitosa" << std::endl;
}

void test_busqueda_avanzada() {
  std::cout << "\n=== TEST BÚSQUEDA AVANZADA ===" << std::endl;

  CList lista;

  // Insertar elementos
  Data d1{"topic1", 1}, d2{"topic2", 1}, d3{"topic3", 1};
  lista.Ins(d1);
  lista.Ins(d2);
  lista.Ins(d3);

  // Test Find (método interno)
  Node *testNode = new Node(Data{"topic2", 1});
  Node *foundNode = testNode;
  bool found = lista.Find(foundNode);

  assert(found == true);
  assert(foundNode->data.topic == "topic2");
  assert(foundNode != testNode); // Debe apuntar al nodo existente
  delete testNode;               // Limpiar memoria
  std::cout << "✓ Find interno funciona" << std::endl;

  // Test Find con elemento no existente
  testNode = new Node(Data{"topic99", 1});
  foundNode = testNode;
  found = lista.Find(foundNode);

  assert(found == false);
  assert(foundNode == testNode); // No debe cambiar el puntero
  delete testNode;
  std::cout << "✓ Find con elemento no existente" << std::endl;

  std::cout << "✅ TEST BÚSQUEDA AVANZADA PASADO" << std::endl;
}

void test_limpieza_lista() {
  std::cout << "\n=== TEST LIMPIEZA LISTA ===" << std::endl;

  CList lista;

  // Llenar la lista
  Data d1{"topic1", 1}, d2{"topic2", 1}, d3{"topic3", 1};
  lista.Ins(d1);
  lista.Ins(d2);
  lista.Ins(d3);
  lista.Ins(d1); // Duplicado

  assert(lista.size == 3);
  assert(lista.total == 4);

  // Limpiar lista
  lista.limpiar_lista();

  assert(lista.size == 0);
  assert(lista.total == 0);
  assert(lista.root == nullptr);
  std::cout << "✓ Limpieza completa de lista" << std::endl;

  // Verificar que se puede usar después de limpiar
  Data d4{"topic4", 1};
  lista.Ins(d4);
  assert(lista.size == 1);
  assert(lista.root->data.topic == "topic4");
  std::cout << "✓ Reutilización después de limpiar" << std::endl;

  std::cout << "✅ TEST LIMPIEZA LISTA PASADO" << std::endl;
}

void test_estres() {
  std::cout << "\n=== TEST ESTRÉS ===" << std::endl;

  CList lista;

  // Insertar muchos elementos
  for (int i = 0; i < 100; i++) {
    Data d{"topic" + std::to_string(i), 1};
    lista.Ins(d);
  }

  assert(lista.size == 100);
  assert(lista.total == 100);
  std::cout << "✓ Inserción masiva (100 elementos)" << std::endl;

  // Mezclar inserciones duplicadas
  for (int i = 0; i < 50; i++) {
    Data d{"topic" + std::to_string(i), 1};
    lista.Ins(d);
  }

  assert(lista.size == 100);  // Tamaño no cambia (solo frecuencia)
  assert(lista.total == 150); // Total sí incrementa
  std::cout << "✓ Inserción con duplicados masivos" << std::endl;

  // Verificar algunas frecuencias
  Data check{"topic0", 1};
  Node *testNode = new Node(check);
  Node *foundNode = testNode;
  lista.Find(foundNode);
  assert(foundNode->data.frq == 2); // Insertado 2 veces
  delete testNode;
  std::cout << "✓ Frecuencias correctas" << std::endl;

  // Limpiar
  lista.limpiar_lista();
  assert(lista.size == 0);
  std::cout << "✓ Limpieza después de estrés" << std::endl;

  std::cout << "✅ TEST ESTRÉS PASADO" << std::endl;
}

void test_impresion() {
  std::cout << "\n=== TEST IMPRESIÓN ===" << std::endl;

  CList lista;

  std::cout << "Lista vacía: ";
  lista.PrintList();
  std::cout << std::endl;

  // Agregar elementos
  Data d1{"topicA", 1}, d2{"topicB", 3}, d3{"topicC", 2};
  lista.Ins(d1);
  lista.Ins(d2);
  lista.Ins(d3);

  std::cout << "Lista con elementos: ";
  lista.PrintList();
  std::cout << std::endl;

  // Agregar duplicado
  lista.Ins(d1);
  std::cout << "Lista con duplicado: ";
  lista.PrintList();
  std::cout << std::endl;

  std::cout << "✅ TEST IMPRESIÓN COMPLETADO" << std::endl;
}

int main() {
  try {
    test_lista_basica();
    test_insercion_duplicados();
    test_eliminacion();
    test_busqueda_avanzada();
    test_limpieza_lista();
    test_estres();
    test_impresion();

    std::cout << "\n🎉 ¡TODOS LOS TESTS DE LISTA PASARON EXITOSAMENTE!"
              << std::endl;
    return 0;
  } catch (const std::exception &e) {
    std::cerr << "❌ Error en test: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "❌ Error desconocido en test" << std::endl;
    return 1;
  }
}