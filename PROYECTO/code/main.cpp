#include "init.h"
#include <chrono>
#include <iostream>

using namespace std;

int main() {
  // INICIAR CRONÓMETRO
  auto inicio = chrono::high_resolution_clock::now();

  Init init = {2, 5, 5, 3, 0};
  CInit app(init);
  app.runtest();
  // app.print();

  // DETENER CRONÓMETRO Y CALCULAR
  auto fin = chrono::high_resolution_clock::now();
  auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);

  cout << "\n⏱️  ====== ESTADÍSTICAS DE TIEMPO ======" << endl;
  cout << "Tiempo total de ejecución: " << duracion.count() << " ms" << endl;
  cout << "Tiempo total de ejecución: " << (duracion.count() / 1000.0)
       << " segundos" << endl;
  cout << "Tiempo total de ejecución: " << (duracion.count() / 60000.0)
       << " minutos" << endl;

  return 0;
}