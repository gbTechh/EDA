#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Punto {
  double x;
  double y;
};

struct Node {
  vector<double> point;
  int centroidIndex; // Para saber a qué centroide corresponde
  Node *left;
  Node *right;
  Node(const vector<double> &p, int idx) {
    point = p;
    centroidIndex = idx;
    left = nullptr;
    right = nullptr;
  }
};

class KDTree {
private:
  Node *root;
  int k;

  Node *insertRec(Node *node, const vector<double> &point, int idx, int depth) {
    if (node == nullptr) {
      return new Node(point, idx);
    }
    int axis = depth % k;
    if (point[axis] < node->point[axis]) {
      node->left = insertRec(node->left, point, idx, depth + 1);
    } else {
      node->right = insertRec(node->right, point, idx, depth + 1);
    }
    return node;
  }

  double dist2(const vector<double> &a, const vector<double> &b) {
    double d = 0;
    for (int i = 0; i < k; i++) {
      double x = a[i] - b[i];
      d += x * x;
    }
    return d;
  }

  void nearestRec(Node *node, const vector<double> &target, int depth,
                  int &bestIdx, double &bestDist) {
    if (!node) {
      return;
    }

    double d = dist2(target, node->point);

    if (d < bestDist) {
      bestDist = d;
      bestIdx = node->centroidIndex;
    }

    int axis = depth % k;
    Node *next = (target[axis] < node->point[axis]) ? node->left : node->right;
    Node *other = (next == node->left) ? node->right : node->left;

    nearestRec(next, target, depth + 1, bestIdx, bestDist);

    if (fabs(target[axis] - node->point[axis]) < sqrt(bestDist)) {
      nearestRec(other, target, depth + 1, bestIdx, bestDist);
    }
  }

public:
  KDTree(int dimensions) {
    root = nullptr;
    k = dimensions;
  }

  void insert(const vector<double> &point, int idx) {
    root = insertRec(root, point, idx, 0);
  }

  int nearestIndex(const vector<double> &target) {
    int bestIdx = -1;
    double bestDist = numeric_limits<double>::max();
    nearestRec(root, target, 0, bestIdx, bestDist);
    return bestIdx;
  }
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
      continue;

    std::stringstream ss(linea);
    std::string parte1, parte2;

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

class KMeansKDTree {
private:
  int k;
  vector<Punto> k_center;
  vector<vector<Punto>> v_clustering;
  vector<Punto> puntos;

  Punto calculateMedia(vector<Punto> v_puntos) {
    Punto new_p;
    double x = 0.0, y = 0.0;
    for (int i = 0; i < v_puntos.size(); i++) {
      x += v_puntos[i].x;
      y += v_puntos[i].y;
    }

    new_p.x = x / v_puntos.size();
    new_p.y = y / v_puntos.size();

    return new_p;
  }

  bool centroidesConvergieron(const vector<Punto> &prevCenters,
                              double epsilon = 1e-6) {
    for (int i = 0; i < k; i++) {
      double dx = k_center[i].x - prevCenters[i].x;
      double dy = k_center[i].y - prevCenters[i].y;
      if (sqrt(dx * dx + dy * dy) > epsilon)
        return false;
    }
    return true;
  }

  // AQUÍ ESTÁ EL CAMBIO PRINCIPAL: Usando KD-Tree en lugar de fuerza bruta
  void pointDisCenter() {
    // Construir el KD-Tree con los centroides actuales
    KDTree tree(2); // 2 dimensiones (x, y)

    for (int i = 0; i < k_center.size(); i++) {
      vector<double> centroid = {k_center[i].x, k_center[i].y};
      tree.insert(centroid, i);
    }

    // Asignar cada punto al centroide más cercano usando el KD-Tree
    for (const auto &p : puntos) {
      vector<double> punto = {p.x, p.y};
      int k_index = tree.nearestIndex(punto);
      v_clustering[k_index].push_back(p);
    }
  }

public:
  KMeansKDTree() {};
  KMeansKDTree(int _k, vector<Punto> data) {
    k = _k;
    puntos = data;

    if (data.empty()) {
      cerr << "Error: no se puede inicializar KMeansKDTree con data vacía.\n";
      return;
    }

    if (k > data.size()) {
      cerr << "Advertencia: k (" << k << ") es mayor que el número de puntos ("
           << data.size() << "). Se ajustará k = " << data.size() << ".\n";
      k = data.size();
    }

    k_center.resize(k);
    v_clustering.resize(k);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, (int)data.size() - 1);

    for (int i = 0; i < k; i++) {
      int num = distrib(gen);
      k_center[i] = data[num];
    }
  }

  void recalculateCentroides() {
    for (int i = 0; i < k; i++) {
      if (v_clustering[i].empty()) {
        continue;
      }
      k_center[i] = calculateMedia(v_clustering[i]);
    }
  }

  void entrenar(int maxIter = 100) {
    for (int iter = 0; iter < maxIter; iter++) {
      vector<Punto> prevCenters = k_center;

      for (auto &cluster : v_clustering) {
        cluster.clear();
      }

      pointDisCenter();
      recalculateCentroides();

      if (centroidesConvergieron(prevCenters)) {
        cout << "Convergencia alcanzada en iteración " << iter + 1 << endl;
        break;
      }
    }
  }

  void imprimirResultados() {
    for (int i = 0; i < k; i++) {
      cout << "Cluster " << i + 1 << " -> Centroide: (" << k_center[i].x << ", "
           << k_center[i].y << "), puntos: " << v_clustering[i].size() << endl;
    }
  }

  void preguntaB(const string &nombreArchivoCSV, int numIteraciones = 10) {
    ofstream archivo(nombreArchivoCSV);
    if (!archivo.is_open()) {
      cerr << "Error al crear el archivo CSV." << endl;
      return;
    }

    archivo << "version,n_puntos,n_clusters,tiempo_ms,ite\n";

    for (int i = 1; i <= numIteraciones; i++) {
      auto start = chrono::high_resolution_clock::now();

      entrenar();

      auto end = chrono::high_resolution_clock::now();
      chrono::duration<double, std::milli> duracion = end - start;

      archivo << "KMeansKDTree," << puntos.size() << "," << k << ","
              << duracion.count() << "," << i << "\n";

      cout << "Iteración " << i << " completada en " << duracion.count()
           << " ms" << endl;
    }

    archivo.close();
    cout << "Resultados guardados en " << nombreArchivoCSV << endl;
  }

  void preguntaC(const std::string &nombreArchivoDatos,
                 const std::string &nombreArchivoSalida) {
    using namespace std;
    using namespace std::chrono;

    vector<int> ks = {5, 15, 25, 50, 75};
    vector<int> ns = {1000, 1150, 1300, 1450, 1600,
                      1750, 1900, 2050, 2200, 2400};

    vector<Punto> data = leerPuntosCSV(nombreArchivoDatos);
    if (data.empty()) {
      cerr << "Error: no se pudo leer el archivo " << nombreArchivoDatos
           << endl;
      return;
    }

    ofstream csv(nombreArchivoSalida);
    csv << fixed << setprecision(4);
    csv << "version,n_puntos,n_clusters,tiempo_ms\n";

    for (int k : ks) {
      for (int n : ns) {
        if (n > data.size())
          continue;

        vector<Punto> subset(data.begin(), data.begin() + n);

        KMeansKDTree kmeans(k, subset);

        auto inicio = high_resolution_clock::now();
        kmeans.entrenar(100);
        auto fin = high_resolution_clock::now();

        double duracion = duration<double, std::milli>(fin - inicio).count();

        csv << "KMeansKDTree" << "," << n << "," << k << "," << duracion
            << "\n";

        cout << "Versión KMeansKDTree completada (k=" << k << ", n=" << n
             << ", tiempo=" << duracion << " ms)\n";
      }
    }

    csv.close();
    cout << "\nPregunta C completada. Resultados guardados en: "
         << nombreArchivoSalida << endl;
  }

  void preguntaD(const string &nombreArchivoDatos,
                 const string &nombreArchivoSalida) {

    vector<int> ks = {5, 15, 25, 50, 75, 100, 125, 150, 200};
    vector<int> ns = {1000, 1450, 1900, 2400};

    vector<Punto> data = leerPuntosCSV(nombreArchivoDatos);
    if (data.empty()) {
      cerr << "Error: no se pudo leer el archivo " << nombreArchivoDatos
           << endl;
      return;
    }

    ofstream csv(nombreArchivoSalida);
    if (!csv.is_open()) {
      cerr << "Error al crear el archivo " << nombreArchivoSalida << endl;
      return;
    }

    csv << "version,n_puntos,n_clusters,tiempo_ms\n";
    csv << std::fixed << std::setprecision(4);

    for (int n : ns) {
      if (n > data.size())
        continue;

      vector<Punto> subset(data.begin(), data.begin() + n);

      for (int k : ks) {

        KMeansKDTree kmeans(k, subset);

        auto inicio = chrono::high_resolution_clock::now();
        kmeans.entrenar(100);
        auto fin = chrono::high_resolution_clock::now();

        chrono::duration<double, std::milli> duracion = fin - inicio;

        csv << "KMeansKDTree" << "," << n << "," << k << "," << duracion.count()
            << "\n";

        cout << "k=" << k << ", n=" << n << " completado en "
             << duracion.count() << " ms\n";
      }
    }

    csv.close();
    cout << "\nPregunta D. Resultados guardados en: " << nombreArchivoSalida
         << endl;
  }
};

int main() {
  std::string archivo = "data2k.csv";
  auto puntos = leerPuntosCSV(archivo);
  int k = 18;

  // Prueba con KMeansKDTree
  KMeansKDTree kmeansKD(k, puntos);
  kmeansKD.preguntaB("B_KDTree.csv", 10);

  KMeansKDTree kmeansKD2;
  kmeansKD2.preguntaC("data2k.csv", "C_KDTree.csv");

  KMeansKDTree kmeansKD3;
  kmeansKD3.preguntaD("data2k.csv", "D_KDTree.csv");

  return 0;
}