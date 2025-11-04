#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Punto {
  double x;
  double y;
};

std::vector<Punto> leerPuntosCSV(const std::string &nombreArchivo) {
  std::vector<Punto> puntos;
  std::ifstream archivo(nombreArchivo);

  if (!archivo.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo
              << std::endl;
    return puntos;
  }

  std::string linea;
  while (std::getline(archivo, linea)) {
    if (linea.empty())
      continue; // ignora líneas vacías

    std::stringstream ss(linea);
    std::string parte1, parte2;

    // Divide la línea en dos partes usando la coma como delimitador
    if (std::getline(ss, parte1, ',') && std::getline(ss, parte2, ',')) {
      try {
        double x = std::stod(parte1);
        double y = std::stod(parte2);
        puntos.push_back({x, y});
      } catch (const std::exception &e) {
        std::cerr << "Advertencia: línea no válida: " << linea << std::endl;
      }
    }
  }

  archivo.close();
  return puntos;
}

class KMeansFB {
private:
  int k;
  vector<Punto> k_center;
  vector<Punto> puntos;

public:
  KMeansFB(int _k, vector<Punto> data) {
    k = _k;
    k_center.resize(k);
    puntos = data;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, data.size() - 2);

    // seleccionar centroides
    for (int i = 0; i < _k; i++) {
      int num = distrib(gen);
      k_center[i] = data[num];
    }
  }
};

int main() {
  std::string archivo = "datos.csv";
  auto puntos = leerPuntosCSV(archivo);

  return 0;
}