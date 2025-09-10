#include "octree.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

string nombreArchivo = "aguila.xyz";

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

  cout << "Archivo xyz leido\n";

  archivo.close();
  float h = processor.getH();
  Point bottomLeft = processor.getBottomLeft();

  Octree root(bottomLeft, h);
  for (const Point &p : processor.getVPoints()) {
    root.insert(p);
  }

  root.exportOBJ("octree.obj");

  // cout << "h:" << h <<"\n";
  // cout << "bootom left: " << bottomLeft.x << "," << bottomLeft.y << "," <<
  // bottomLeft.z << "\n"; root.printTree();
  Point closest = root.find_closest({1, 2, 1}, 0.5);

  Point nodeBottomLeft(0, 0, 0);
  float nodeH = 0.0f;
  root.findNodeForPoint(closest, nodeBottomLeft, nodeH);

  std::cout << "Punto encontrado: (" << closest.x << ", " << closest.y << ", "
            << closest.z << ")\n";
  std::cout << "Nodo que lo contiene: bottomLeft = (" << nodeBottomLeft.x
            << ", " << nodeBottomLeft.y << ", " << nodeBottomLeft.z
            << "), h = " << nodeH << std::endl;
  // cout << "point: " << p.x << "," << p.y << "," << p.z <<"Es el mismo punto,
  // no hay otro" << "\n";

  return 0;
}