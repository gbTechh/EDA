#include "octree.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string nombreArchivo = "txt.xyz";

int main() {
  std::ifstream archivo(nombreArchivo);

  if (!archivo.is_open()) {
    std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo
              << std::endl;
    return 0;
  }

  std::string linea;
  Octree processor;

  while (std::getline(archivo, linea)) {
    // Saltar líneas vacías
    if (linea.empty())
      continue;

    std::istringstream ss(linea);
    double x, y, z;

    if (ss >> x >> y >> z) {
      processor.readPoints(x, y, z);
    } else {
      std::cerr << "Advertencia: Línea con formato incorrecto: " << linea
                << std::endl;
    }
  }

  archivo.close();
  float h = processor.getH();
  Point bottomLeft = processor.getBottomLeft();

  Octree root(bottomLeft, h);
  for (const Point &p : processor.getVPoints()) {
    root.insert(p);
  }

  root.printTree();
  return 0;
}