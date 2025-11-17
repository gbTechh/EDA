#include "../map.h" // Asegúrate de tener la ruta correcta a tu map.h
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

// Implementacion de Fd_String y Fd_Simple (funciones hash)
template <class T> struct Fd_String {
  inline unsigned long operator()(const T &x) const {
    unsigned long hash_value = 0;
    int i = 1;
    for (char c : x) {
      hash_value += static_cast<unsigned long>(c) * i + i;
      i++;
    }
    return hash_value;
  }
};
template <class T> struct Fd_Simple {
  inline unsigned long operator()(const T &x) const {
    // Un simple hash multiplicativo para enteros
    return static_cast<unsigned long>(x) * 2654435761UL;
  }
};

const unsigned long TABLE_SIZE = 10;

// Test con tipos simples: K=int, V=double
void test_int_double() {
  std::cout << "=== TEST K=int, V=double ===" << std::endl;
  CMap<int, double, Fd_Simple<int>, TABLE_SIZE> mapa_hash;

  mapa_hash.ins(10, 100.5);
  mapa_hash.ins(20, 200.5);
  mapa_hash.ins(30, 300.5);

  std::cout << "✓ Ins exitosos" << std::endl;

  // Test findk (buscar key) - Sigue funcionando igual
  assert(mapa_hash.findk(10) == true);
  assert(mapa_hash.findk(999) == false);
  std::cout << "✓ findk exitoso" << std::endl;

  // Test find (verifica y obtiene el valor, usando try-catch para error)
  try {
    double valor_10 = mapa_hash.find(10);
    assert(valor_10 == 100.5);
    std::cout << "✓ find (existencia y valor OK)" << std::endl;
  } catch (const std::out_of_range &e) {
    assert(false &&
           "find(10) deberia haber encontrado la clave y no lanzar excepcion");
  }

  // Test find con clave inexistente (debe lanzar excepción)
  try {
    mapa_hash.find(999); // Esto debería lanzar
    assert(false &&
           "find(999) deberia haber lanzado std::out_of_range"); // Si llegamos
                                                                 // aqui, falla
                                                                 // el test
  } catch (const std::out_of_range &e) {
    std::cout << "✓ find (clave inexistente lanza excepcion OK)" << std::endl;
  }

  std::cout << "Estado inicial de la tabla:" << std::endl;
  mapa_hash.PrintTable();

  // ... (Test Rem y limpieza del resto, que no cambiaron) ...
  assert(mapa_hash.rem(20) == true);
  assert(mapa_hash.findk(20) == false);
  assert(mapa_hash.rem(999) == false);
  assert(mapa_hash.rem(10) == true);
  assert(mapa_hash.rem(30) == true);

  std::cout << "✅ TEST K=int, V=double PASADO" << std::endl;
}

// Test con tipos complejos: K=string, V=string
void test_string_string() {
  std::cout << "\n=== TEST K=string, V=string ===" << std::endl;

  CMap<std::string, std::string, Fd_String<std::string>, TABLE_SIZE> mapa_hash;

  mapa_hash.ins("frutas", "manzana");
  mapa_hash.ins("vegetales", "zanahoria");
  mapa_hash.ins("bebida", "agua");
  std::cout << "✓ Ins exitoso" << std::endl;

  // Verificar usando findk
  assert(mapa_hash.findk("frutas") == true);
  assert(mapa_hash.findk("bebida") == true);
  assert(mapa_hash.findk("carne") == false);
  std::cout << "✓ findk exitoso" << std::endl;

  std::cout << "Estado inicial de la tabla:" << std::endl;
  mapa_hash.PrintTable();

  // Test Rem
  assert(mapa_hash.rem("vegetales") == true);
  assert(mapa_hash.findk("vegetales") == false);
  std::cout << "✓ Rem exitoso" << std::endl;

  std::cout << "Estado de la tabla despues de remover 'vegetales':"
            << std::endl;
  mapa_hash.PrintTable();

  // Limpiar el resto
  assert(mapa_hash.rem("frutas") == true);
  assert(mapa_hash.rem("bebida") == true);
  assert(mapa_hash.findk("frutas") == false);
  assert(mapa_hash.findk("bebida") == false);

  std::cout << "✅ TEST K=string, V=string PASADO" << std::endl;
}

void test_string_Cvector() {
  std::cout << "\n=== TEST K=string, V=CVector<std::string> ===" << std::endl;

  // Tipo de mapa que usa CVector<std::string> como valor (V)
  CMap<std::string, CVector<std::string>, Fd_String<std::string>, TABLE_SIZE>
      mapa_cvector;

  // Crear instancias de CVector
  CVector<std::string> frutas;
  frutas.push_back("manzana");
  frutas.push_back("banana");
  frutas.push_back("cereza");

  CVector<std::string> vegetales;
  vegetales.push_back("zanahoria");
  vegetales.push_back("brocoli");

  // Insertar en el mapa hash
  mapa_cvector.ins("Frutas", frutas);
  mapa_cvector.ins("Vegetales", vegetales);
  std::cout << "✓ Ins exitoso de CVector" << std::endl;

  // Verificar usando findk
  assert(mapa_cvector.findk("Frutas") == true);
  assert(mapa_cvector.findk("Vegetales") == true);
  assert(mapa_cvector.findk("Carnes") == false);
  std::cout << "✓ findk exitoso" << std::endl;

  // Validar el contenido del CVector almacenado
  // Utilizamos get() para obtener el puntero al KeyValue
  unsigned long index_frutas = mapa_cvector.fd("Frutas") % TABLE_SIZE;
  const KeyValue<std::string, CVector<std::string>> *kv_frutas =
      mapa_cvector.bucket[index_frutas].get("Frutas");

  assert(kv_frutas != nullptr);
  assert(kv_frutas->value.size() == 3);
  assert(kv_frutas->value[0] ==
         "manzana"); // Acceso usando operator[] de CVector
  assert(kv_frutas->value[2] == "cereza");
  std::cout << "✓ Contenido del CVector 'Frutas' verificado" << std::endl;

  // Test Rem
  assert(mapa_cvector.rem("Vegetales") == true);
  assert(mapa_cvector.findk("Vegetales") == false);
  std::cout << "✓ Rem exitoso" << std::endl;

  mapa_cvector.PrintTable();

  std::cout << "✅ TEST K=string, V=CVector<std::string> PASADO" << std::endl;
}

int main() {
  test_int_double();
  test_string_string();
  test_string_Cvector();

  std::cout << "\nTodos los tests de CMap pasaron exitosamente." << std::endl;
  return 0;
}
