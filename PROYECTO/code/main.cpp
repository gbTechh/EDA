#include "init.h"
#include <chrono>
#include <iostream>

using namespace std;

// CONSTANTES GLOBALES CONTROLADAS DESDE MAIN
const unsigned long HASH_SIZE_VENTANA = 30011;
const unsigned long HASH_SIZE_CEMENTERIO = 99991;
const int BUCKET_SIZE = 200;

// cosntantes para el topic
const int K = 100;
const int DOCUMENTOS_VENTANA = 1000;
int TIMESLEEP_MS = 100;

// cosntante para app.runtest(objetivo_documentos)
const long long OBJETIVO_DOCUMENTOS = 100000;

int main() {
  auto inicio = chrono::high_resolution_clock::now();

  Init init = {K, BUCKET_SIZE, DOCUMENTOS_VENTANA, 0};
  CInit<HASH_SIZE_VENTANA, HASH_SIZE_CEMENTERIO> app(init);
  app.runtest(OBJETIVO_DOCUMENTOS, TIMESLEEP_MS);

  auto fin = chrono::high_resolution_clock::now();
  auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);

  cout << "\n⏱️  ====== ESTADÍSTICAS DE TIEMPO ======" << endl;
  cout << "Tiempo total de ejecución: " << duracion.count() << " ms" << endl;
  cout << "Tiempo total de ejecución: " << (duracion.count() / 1000.0)
       << " segundos" << endl;
  cout << "Tiempo total de ejecución: " << (duracion.count() / 60000.0)
       << " minutos" << endl;
  app.print();

  return 0;
}