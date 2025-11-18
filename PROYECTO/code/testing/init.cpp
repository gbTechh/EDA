#include "../init.h"
#include <iostream>
#include <string>

using namespace std;

// CONSTANTES GLOBALES CONTROLADAS DESDE MAIN
const unsigned long HASH_SIZE_VENTANA = 101;
const unsigned long HASH_SIZE_CEMENTERIO = 101;
const int K = 3;
const int BUCKET_SIZE = 10;
const int DOCUMENTOS_VENTANA = 3;

void test_caso_basico() {
  cout << "=== TEST CASO BÁSICO ===" << endl;

  Init init = {K, BUCKET_SIZE, DOCUMENTOS_VENTANA, 0};
  CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app(init);

  // Definir documentos de test
  CVector<CVector<string>> documentos_test;

  // Documento 1: "apple" aparece 3 veces
  CVector<string> doc1;
  doc1.push_back("apple");
  doc1.push_back("apple");
  doc1.push_back("banana");
  doc1.push_back("apple");
  documentos_test.push_back(doc1);

  // Documento 2: "apple" aparece 1 vez, "cherry" 2 veces
  CVector<string> doc2;
  doc2.push_back("apple");
  doc2.push_back("cherry");
  doc2.push_back("cherry");
  documentos_test.push_back(doc2);

  // Documento 3: "date" y "elderberry"
  CVector<string> doc3;
  doc3.push_back("date");
  doc3.push_back("elderberry");
  documentos_test.push_back(doc3);

  // Documento 4: "apple" y "fig" - debería trigger ventana deslizante
  CVector<string> doc4;
  doc4.push_back("apple");
  doc4.push_back("fig");
  documentos_test.push_back(doc4);

  // Documento 5: "grape" aparece 3 veces
  CVector<string> doc5;
  doc5.push_back("grape");
  doc5.push_back("grape");
  doc5.push_back("grape");
  documentos_test.push_back(doc5);

  cout << "📊 Esperado: Después de procesar 5 documentos con ventana=3" << endl;
  cout << "   - Doc1: apple(3), banana(1)" << endl;
  cout << "   - Doc2: apple(4), cherry(2), banana(1)" << endl;
  cout << "   - Doc3: apple(4), cherry(2), banana(1), date(1), elderberry(1)"
       << endl;
  cout << "   - Doc4: VENTANA DESLIZADA - se remueve Doc1" << endl;
  cout << "           apple(2), cherry(2), date(1), elderberry(1), fig(1)"
       << endl;
  cout << "   - Doc5: apple(2), cherry(2), grape(3)" << endl;
  cout << "   RESULTADO: [grape(3), apple(2), cherry(2)]" << endl << endl;

  app.runtestvector(documentos_test);
}

void test_estancamiento_heap() {
  cout << "\n=== TEST EVITAR ESTANCAMIENTO HEAP ===" << endl;

  Init init = {2, 5, 3, 0}; // k=2, ventana=3
  CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app(init);

  CVector<CVector<string>> documentos_test;

  // Llenar inicial con tokens de baja frecuencia
  CVector<string> doc1;
  doc1.push_back("token1");
  doc1.push_back("token2");
  documentos_test.push_back(doc1);

  CVector<string> doc2;
  doc2.push_back("token3");
  doc2.push_back("token4");
  documentos_test.push_back(doc2);

  CVector<string> doc3;
  doc3.push_back("token5");
  doc3.push_back("token6");
  documentos_test.push_back(doc3);

  // Añadir tokens de alta frecuencia que deberían reemplazar a los anteriores
  CVector<string> doc4;
  doc4.push_back("high_freq");
  doc4.push_back("high_freq");
  doc4.push_back("high_freq"); // frecuencia 3
  documentos_test.push_back(doc4);

  CVector<string> doc5;
  doc5.push_back("another_high");
  doc5.push_back("another_high"); // frecuencia 2
  documentos_test.push_back(doc5);

  cout << "📊 Esperado: Heap no debería estancarse con tokens de frecuencia 1"
       << endl;
  cout << "   Debería mostrar: [high_freq(3), another_high(2)]" << endl << endl;

  app.runtestvector(documentos_test);
}

void test_rem_freq_comportamiento() {
  cout << "\n=== TEST COMPORTAMIENTO rem_freq ===" << endl;

  Init init = {2, 10, 2, 0}; // k=2, ventana=2
  CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app(init);

  CVector<CVector<string>> documentos_test;

  // Documento 1: "leader" aparece 3 veces
  CVector<string> doc1;
  doc1.push_back("leader");
  doc1.push_back("leader");
  doc1.push_back("leader");
  doc1.push_back("follower");
  documentos_test.push_back(doc1);

  // Documento 2: "follower" aparece 2 veces
  CVector<string> doc2;
  doc2.push_back("follower");
  doc2.push_back("follower");
  documentos_test.push_back(doc2);

  // Documento 3: Trigger ventana deslizante - remueve Doc1
  CVector<string> doc3;
  doc3.push_back("new_token");
  documentos_test.push_back(doc3);

  cout << "📊 Esperado: rem_freq debería decrementar frecuencias correctamente"
       << endl;
  cout << "   - Doc1-2: [leader(3), follower(3)]" << endl;
  cout << "   - Doc3: VENTANA DESLIZADA - remueve Doc1" << endl;
  cout << "           leader(0), follower(2), new_token(1)" << endl;
  cout << "   RESULTADO: [follower(2), new_token(1)]" << endl << endl;

  app.runtestvector(documentos_test);
}

int main() {
  cout << "🧪 INICIANDO TESTS CON VECTOR PREDEFINIDO 🧪" << endl << endl;

  test_caso_basico();
  test_estancamiento_heap();
  test_rem_freq_comportamiento();

  cout << "\n🎉 TODOS LOS TESTS CON VECTOR COMPLETADOS" << endl;
  return 0;
}