#include "init.h"
#include <chrono>
#include <iostream>

using namespace std;

// CONSTANTES GLOBALES CONTROLADAS DESDE MAIN
const unsigned long HASH_SIZE_VENTANA = 30011;
const unsigned long HASH_SIZE_CEMENTERIO = 99991;
const int K = 2;
const int BUCKET_SIZE = 100;
const int DOCUMENTOS_VENTANA = 100;
const long long OBJETIVO_DOCUMENTOS = 100000;

CVector<CVector<std::string>> crearTestVector() {
  CVector<CVector<std::string>> documentos_test;

  // Documento A: Primera ventana
  CVector<std::string> docA;
  docA.push_back("camion");
  docA.push_back("trump");
  docA.push_back("musica");
  docA.push_back("deporte");
  documentos_test.push_back(docA);

  // Documento B: Primera ventana
  CVector<std::string> docB;
  docB.push_back("trump");
  docB.push_back("politica");
  docB.push_back("camion");
  docB.push_back("economia");
  documentos_test.push_back(docB);

  // Documento C: Primera ventana
  CVector<std::string> docC;
  docC.push_back("musica");
  docC.push_back("arte");
  docC.push_back("trump");
  docC.push_back("camion");
  documentos_test.push_back(docC);

  // Documento D: Segunda ventana (reemplaza a A)
  CVector<std::string> docD;
  docD.push_back("nuevo");
  docD.push_back("tecnologia");
  docD.push_back("trump");
  docD.push_back("ciencia");
  documentos_test.push_back(docD);

  // Documento E: Segunda ventana (reemplaza a B)
  CVector<std::string> docE;
  docE.push_back("tecnologia");
  docE.push_back("nuevo");
  docE.push_back("innovacion");
  docE.push_back("ciencia");
  documentos_test.push_back(docE);

  // Documento F: Segunda ventana (reemplaza a C)
  CVector<std::string> docF;
  docF.push_back("arte");
  docF.push_back("musica");
  docF.push_back("nuevo");
  docF.push_back("cultura");
  documentos_test.push_back(docF);

  return documentos_test;
}

int main() {
  // auto inicio = chrono::high_resolution_clock::now();

  //   Init init = {K, BUCKET_SIZE, DOCUMENTOS_VENTANA, 0};
  //   CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app(init);
  //   app.runtest(OBJETIVO_DOCUMENTOS);

  //   auto fin = chrono::high_resolution_clock::now();
  //   auto duracion = chrono::duration_cast<chrono::milliseconds>(fin -
  //   inicio);

  //   cout << "\n⏱️  ====== ESTADÍSTICAS DE TIEMPO ======" << endl;
  //   cout << "Tiempo total de ejecución: " << duracion.count() << " ms" <<
  //   endl; cout << "Tiempo total de ejecución: " << (duracion.count() /
  //   1000.0)
  //        << " segundos" << endl;
  //   cout << "Tiempo total de ejecución: " << (duracion.count() / 60000.0)
  //        << " minutos" << endl;
  //   app.print();
  std::cout << "🧪 EJECUTANDO TEST CON VECTOR PREDEFINIDO" << std::endl;

  Init init_test = {K, BUCKET_SIZE, 3, 0}; // Ventana de 3 documentos
  CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app_test(init_test);

  CVector<CVector<std::string>> test_docs = crearTestVector();
  app_test.runtestvector2(test_docs);

  std::cout << "\n\n🚀 EJECUTANDO VERSIÓN COMPLETA" << std::endl;

  return 0;
}