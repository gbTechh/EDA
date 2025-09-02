#include <fstream>
#include <iomanip>

int main() {
  std::ofstream outFile("points.txt");
  if (!outFile.is_open()) {
    return 1;
  }

  // Generar puntos desde (0.0, 0.0, 0.0) hasta (10.0, 10.0, 10.0)
  for (int x = 0; x <= 10; ++x) {
    for (int y = 0; y <= 10; ++y) {
      for (int z = 0; z <= 10; ++z) {
        // Escribir coordenadas en formato flotante con un decimal
        outFile << std::fixed << std::setprecision(1) << x << ".0 " << y
                << ".0 " << z << ".0\n";
      }
    }
  }

  outFile.close();
  return 0;
}