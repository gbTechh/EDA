#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

double random_number();

class Dimension {
private:
  int filas = 100;
  int columnas;
  vector<double> v_punto;
  vector<vector<double>> v_matriz;
  vector<double> v_distancias;

public:
  Dimension(int _columnas)
      : columnas(_columnas), v_matriz(filas, vector<double>(_columnas, 0.0)) {}

  void fill_matrix() {
    for (int i = 0; i < filas; i++) {
      for (int j = 0; j < columnas; j++) {
        v_matriz[i][j] = random_number();
      }
    }
  }

  void print_matrix() {
    cout << "Matriz generada:" << endl;
    for (int i = 0; i < filas; i++) {
      for (int j = 0; j < columnas; j++) {
        cout << fixed << setprecision(2) << v_matriz[i][j] << " ";
      }
      cout << endl;
    }
  }

  void distancias() {
    for (int i = 0; i < filas - 1; i++) {
      for (int k = (i + 1); k < filas; k++) {
        double suma = 0;
        for (int j = 0; j < columnas; j++) {
          double p1 = v_matriz[i][j];
          double p2 = v_matriz[k][j];
          double resta = p1 - p2;
          double cuadrado = resta * resta;
          suma += cuadrado;
        }
        double d = sqrt(suma);
        v_distancias.push_back(d);
      }
    }
  }

  vector<double> getDistanciasVector() { return v_distancias; }
};

int main() {

  srand(static_cast<unsigned int>(time(0)));

  Dimension miDimension(5000);

  miDimension.fill_matrix();

  // miDimension.print_matrix();
  miDimension.distancias();

  vector<double> distancias = miDimension.getDistanciasVector();

  // Abrir archivo para escribir
  std::ofstream outFile("data5000.csv");
  if (!outFile) {
    std::cerr << "Error al abrir el archivo para escritura" << std::endl;
    return 1;
  }

  // Escribir los datos
  for (const auto &value : distancias) {
    outFile << std::fixed << std::setprecision(6) << value << "\n";
  }

  outFile.close();
  std::cout << "Datos exportados a data.csv" << std::endl;
  return 0;
}

double random_number() {
  double point = static_cast<double>(rand()) / RAND_MAX;

  point = round(point * 100) / 100.0;
  return point;
}