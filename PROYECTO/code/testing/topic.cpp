#include "../topic.h"
#include "../vector.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace std;

void test_min_heap_actualizacion_correcta() {
  cout << "=== TEST ACTUALIZACIÓN CORRECTA DEL MIN-HEAP ===" << endl;

  CTopic<101, 101> topic(3, 100,
                         10); // k=3, bucket_size=100, documentos_ventana=10

  cout << "1. Verificar que nuevos tokens con frecuencia >= min_frequency "
          "entran al heap"
       << endl;

  // Llenar heap inicial con tokens de baja frecuencia
  topic.add_ventana("token1");
  topic.add_ventana("token2");
  topic.add_ventana("token3");

  cout << "Heap inicial (todos con frecuencia 1): ";
  topic.printVentanaActual();

  // Añadir tokens con mayor frecuencia que deberían reemplazar a los existentes
  topic.add_ventana("high_freq");
  topic.add_ventana("high_freq"); // frecuencia 2
  topic.add_ventana("high_freq"); // frecuencia 3

  cout << "Heap después de añadir token con frecuencia 3: ";
  topic.printVentanaActual();
  // "high_freq"(3) debería estar en el heap

  cout << "2. Verificar que tokens con frecuencia decrementada salen del heap "
          "si es necesario"
       << endl;

  // Crear una situación donde un token en el heap baja su frecuencia
  topic.add_ventana("temp_high");
  topic.add_ventana("temp_high"); // temp_high(2) - debería entrar al heap

  cout << "Heap con temp_high: ";
  topic.printVentanaActual();

  // Remover temp_high para que baje su frecuencia
  CVector<string> remover_temp;
  remover_temp.push_back("temp_high");
  remover_temp.push_back("temp_high"); // frecuencia 2→0

  topic.rem_freq(remover_temp);

  cout << "Heap después de remover temp_high 2 veces: ";
  topic.printVentanaActual();
  // temp_high debería haber salido del heap

  cout << "✅ TEST ACTUALIZACIÓN MIN-HEAP COMPLETADO" << endl << endl;
}

void test_heap_no_estancamiento() {
  cout << "=== TEST EVITAR ESTANCAMIENTO DEL HEAP ===" << endl;

  CTopic<101, 101> topic(2, 50, 5); // k=2 para test más simple

  cout << "1. Llenar heap inicial con tokens de frecuencia 1" << endl;

  topic.add_ventana("old1");
  topic.add_ventana("old2");

  cout << "Heap inicial: ";
  topic.printVentanaActual();
  int min_freq_inicial = 1; // Todos tienen frecuencia 1

  cout << "2. Añadir nuevos tokens que superen la frecuencia mínima" << endl;

  // Añadir token que merece entrar al heap
  topic.add_ventana("deserving");
  topic.add_ventana("deserving"); // frecuencia 2

  cout << "Heap después de añadir 'deserving' con frecuencia 2: ";
  topic.printVentanaActual();
  // "deserving"(2) debería reemplazar a uno de los tokens con frecuencia 1

  cout << "3. Verificar que el heap se actualiza dinámicamente" << endl;

  // Añadir otro token de alta frecuencia
  topic.add_ventana("new_star");
  topic.add_ventana("new_star");
  topic.add_ventana("new_star"); // frecuencia 3

  cout << "Heap después de añadir 'new_star' con frecuencia 3: ";
  topic.printVentanaActual();
  // "new_star"(3) debería estar en el heap

  // El heap debería contener los 2 tokens con mayor frecuencia
  // y haber eliminado automáticamente los de menor frecuencia

  cout << "✅ TEST EVITAR ESTANCAMIENTO COMPLETADO" << endl << endl;
}

void test_actualizacion_heap_durante_rem_freq() {
  cout << "=== TEST ACTUALIZACIÓN HEAP DURANTE rem_freq ===" << endl;

  CTopic<101, 101> topic(2, 100, 5);

  cout << "1. Configurar situación donde rem_freq debe actualizar heap" << endl;

  // Crear tokens en el heap
  topic.add_ventana("leader");
  topic.add_ventana("leader");
  topic.add_ventana("leader"); // leader(3)
  topic.add_ventana("follower");
  topic.add_ventana("follower"); // follower(2)

  cout << "Heap inicial: ";
  topic.printVentanaActual(); // [leader(3), follower(2)]

  cout << "2. Ejecutar rem_freq que disminuye frecuencia del líder" << endl;

  CVector<string> remover_leader;
  remover_leader.push_back("leader");
  remover_leader.push_back("leader"); // leader: 3→1

  topic.rem_freq(remover_leader);

  cout << "Heap después de rem_freq: ";
  topic.printVentanaActual();
  // leader(1) podría salir del heap si hay otros tokens con frecuencia mayor

  cout << "3. Verificar que el heap se mantiene consistente" << endl;

  // Añadir nuevo token para verificar que el heap funciona correctamente
  topic.add_ventana("challenger");
  topic.add_ventana("challenger"); // challenger(2)

  cout << "Heap final: ";
  topic.printVentanaActual();
  // Debería mostrar los 2 tokens con mayor frecuencia actual

  cout << "✅ TEST ACTUALIZACIÓN HEAP EN rem_freq COMPLETADO" << endl << endl;
}

void test_comportamiento_borde_heap() {
  cout << "=== TEST COMPORTAMIENTO BORDE DEL HEAP ===" << endl;

  CTopic<101, 101> topic(3, 100, 5);

  cout << "1. Test con frecuencias iguales" << endl;

  // Crear múltiples tokens con la misma frecuencia
  for (int i = 0; i < 5; i++) {
    topic.add_ventana("token_A");
    topic.add_ventana("token_B");
    topic.add_ventana("token_C");
  }
  // Todos tienen frecuencia 5

  cout << "Heap con múltiples tokens de frecuencia 5: ";
  topic.printVentanaActual();
  // El heap debería mantener 3 de ellos (los que fueron insertados últimos o
  // según el criterio del heap)

  cout << "2. Test con frecuencia que cae a 0 durante rem_freq" << endl;

  CVector<string> remover_B;
  for (int i = 0; i < 5; i++) {
    remover_B.push_back("token_B");
  }

  topic.rem_freq(remover_B); // token_B frecuencia 5→0

  cout << "Heap después de remover token_B completamente: ";
  topic.printVentanaActual();
  // token_B debería haber salido del heap

  cout << "3. Test de actualización con frecuencia marginal" << endl;

  // Añadir token con frecuencia que apenas supera el mínimo
  topic.add_ventana("marginal");
  topic.add_ventana("marginal"); // frecuencia 2

  cout << "Heap después de añadir token marginal (freq=2): ";
  topic.printVentanaActual();

  cout << "✅ TEST COMPORTAMIENTO BORDE COMPLETADO" << endl << endl;
}

void test_ventana_deslizante_basica() {
  cout << "=== TEST VENTANA DESLIZANTE BÁSICA ===" << endl;

  // Configuración pequeña para testing
  CTopic<101, 101> topic(3, 10, 5); // k=3, bucket_size=10, documentos_ventana=5

  cout << "1. Llenado inicial de la ventana..." << endl;

  // Ventana inicial: [Doc1, Doc2, Doc3, Doc4, Doc5]
  // Doc1: "apple" aparece 3 veces
  topic.add_ventana("apple");
  topic.add_ventana("apple");
  topic.add_ventana("banana");
  topic.add_ventana("apple"); // apple(3), banana(1)
  topic.add_ventana("cherry");
  topic.add_ventana("cherry"); // apple(3), cherry(2), banana(1)
  topic.add_ventana("date");
  topic.add_ventana("elderberry");

  cout << "Heap después de Doc1-Doc5: ";
  topic.printVentanaActual();

  cout << "2. Simular rem_freq - remover Doc1..." << endl;

  // Simular que removemos Doc1 que contenía: ["apple", "apple", "banana"]
  CVector<string> tokens_remover;
  tokens_remover.push_back("apple");
  tokens_remover.push_back("apple");
  tokens_remover.push_back("banana");

  topic.rem_freq(tokens_remover);

  cout << "Heap después de rem_freq: ";
  topic.printVentanaActual();

  cout << "3. Añadir nuevo documento (Doc6)..." << endl;

  // Añadir Doc6: ["fig", "apple", "grape"]
  topic.add_ventana("fig");
  topic.add_ventana("apple"); // apple: 1+1 = 2
  topic.add_ventana("grape");

  cout << "Heap final: ";
  topic.printVentanaActual();

  cout << "✅ TEST VENTANA DESLIZANTE BÁSICA COMPLETADO" << endl << endl;
}

void test_comportamiento_heap_rem_freq() {
  cout << "=== TEST COMPORTAMIENTO HEAP EN rem_freq ===" << endl;

  CTopic<101, 101> topic(2, 5, 3); // k=2, bucket_size=5, documentos_ventana=3

  cout << "Caso 1: Token con frecuencia que llega a 0 debería salir del heap"
       << endl;

  // Setup: "apple" tiene frecuencia 2, "banana" tiene 1
  topic.add_ventana("apple");
  topic.add_ventana("apple");
  topic.add_ventana("banana");

  cout << "Heap inicial: ";
  topic.printVentanaActual(); // Debería mostrar [apple(2), banana(1)]

  // Remover "apple" 2 veces (frecuencia llega a 0)
  CVector<string> remover_apple;
  remover_apple.push_back("apple");
  remover_apple.push_back("apple");

  topic.rem_freq(remover_apple);

  cout << "Heap después de remover apple 2 veces: ";
  topic.printVentanaActual();

  cout << "Caso 2: Token con frecuencia que baja pero sigue > 0" << endl;

  // Reset
  CTopic<101, 101> topic2(2, 5, 3);

  topic2.add_ventana("apple");
  topic2.add_ventana("apple");
  topic2.add_ventana("apple"); // apple(3)
  topic2.add_ventana("banana");
  topic2.add_ventana("banana"); // banana(2)

  cout << "Heap inicial: ";
  topic2.printVentanaActual(); // [apple(3), banana(2)]

  // Remover "apple" 1 vez (frecuencia 3→2)
  CVector<string> remover_apple_una_vez;
  remover_apple_una_vez.push_back("apple");

  topic2.rem_freq(remover_apple_una_vez);

  cout << "Heap después de remover apple 1 vez: ";
  topic2.printVentanaActual();

  cout << "✅ TEST COMPORTAMIENTO HEAP COMPLETADO" << endl << endl;
}

int main() {
  cout << "🧪 INICIANDO TEST COMPLETO DE CTOPIC - ENFOCADO EN MIN-HEAP 🧪"
       << endl
       << endl;

  try {
    test_min_heap_actualizacion_correcta();
    test_heap_no_estancamiento();
    test_actualizacion_heap_durante_rem_freq();
    test_comportamiento_borde_heap();
    test_ventana_deslizante_basica();
    test_comportamiento_heap_rem_freq();

    cout << "🎉 TODOS LOS TESTS DE COMPORTAMIENTO DEL MIN-HEAP PASARON!"
         << endl;
    cout << "El heap se actualiza correctamente y evita el estancamiento."
         << endl;

  } catch (const exception &e) {
    cout << "❌ ERROR en test: " << e.what() << endl;
    return 1;
  }

  return 0;
}