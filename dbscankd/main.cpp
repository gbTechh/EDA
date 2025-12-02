#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Point {
  int num_cluster = -1;
  bool is_visited = false;
  vector<double> v_data;
  int type = 0; // 1 punto nucleo, 2 punto borde, 3 punto ruido
  Point() {}
  Point(const vector<double> &data) : v_data(data) {}
};

struct KDNode {
  vector<double> point;
  int index;
  KDNode *left;
  KDNode *right;

  KDNode(const vector<double> &p, int idx)
      : point(p), index(idx), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
  KDNode *root;
  int k;

  KDNode *insertRec(KDNode *node, const vector<double> &point, int idx,
                    int depth) {
    if (node == nullptr) {
      return new KDNode(point, idx);
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

  void pointsInSphereRec(KDNode *node, const vector<double> &center,
                         double radius2, int depth, vector<int> &result) {
    if (!node)
      return;

    double d = dist2(center, node->point);
    if (d <= radius2) {
      result.push_back(node->index);
    }

    int axis = depth % k;
    double diff = center[axis] - node->point[axis];

    // Decide which subtree to search first
    KDNode *first = (diff < 0) ? node->left : node->right;
    KDNode *second = (first == node->left) ? node->right : node->left;

    pointsInSphereRec(first, center, radius2, depth + 1, result);

    // If the hypersphere crosses the splitting plane, search the other subtree
    if (diff * diff <= radius2) {
      pointsInSphereRec(second, center, radius2, depth + 1, result);
    }
  }

public:
  KDTree(int dimensions) {
    root = nullptr;
    k = dimensions;
  }

  void insert(const vector<double> &point, int index) {
    root = insertRec(root, point, index, 0);
  }

  vector<int> pointsInSphere(const vector<double> &center, double radius) {
    vector<int> result;
    double radius2 = radius * radius;
    pointsInSphereRec(root, center, radius2, 0, result);
    return result;
  }
};

class CDBScan {
public:
  KDTree *kd_tree;
  bool usar_kdtree;
  int dim;
  double epsilon;
  int min_ptrs;
  vector<Point> v_points;
  unordered_map<int, vector<Point>> m_cluster;

public:
  CDBScan(double ep, int min)
      : epsilon(ep), min_ptrs(min), usar_kdtree(false), kd_tree(nullptr) {}
  CDBScan(double ep, int min, const vector<vector<double>> &all_points)
      : epsilon(ep), min_ptrs(min), usar_kdtree(true) {

    kd_tree = new KDTree(all_points[0].size());
    for (int i = 0; i < all_points.size(); i++) {
      kd_tree->insert(all_points[i], i);

      Point p(all_points[i]);
      v_points.push_back(p);
    }
  }

  ~CDBScan() {
    if (kd_tree != nullptr) {
      delete kd_tree;
    }
  }

  void ins(const vector<double> &data) {
    Point p(data);
    v_points.push_back(p);
  }

  double calcular_distancia(const Point &p1, const Point &p2) {
    double sum = 0.0;
    for (int i = 0; i < p1.v_data.size(); i++) {
      sum += (p1.v_data[i] - p2.v_data[i]) * (p1.v_data[i] - p2.v_data[i]);
    }
    return sqrt(sum);
  }

  vector<int> encontrar_vecinos_kdtree(int index) {
    return kd_tree->pointsInSphere(v_points[index].v_data, epsilon);
  }

  vector<int> encontrar_vecinos_lineal(int index) {
    vector<int> vecinos_indices;
    for (int i = 0; i < v_points.size(); i++) {
      if (i == index) {
        continue;
      }

      double dis = calcular_distancia(v_points[i], v_points[index]);
      if (dis <= epsilon) {
        vecinos_indices.push_back(i);
      }
    }

    return vecinos_indices;
  }

  vector<int> encontrar_vecinos(int index) {
    if (usar_kdtree) {
      return encontrar_vecinos_kdtree(index);
    } else {
      return encontrar_vecinos_lineal(index);
    }
  }

  void runScan() {
    int current_cluster_id = 0;
    for (int i = 0; i < v_points.size(); i++) {
      if (v_points[i].is_visited)
        continue;
      vector<int> vecinos_indices = encontrar_vecinos(i);

      if (vecinos_indices.size() >= min_ptrs) {
        v_points[i].type = 1;
        current_cluster_id++;
        v_points[i].num_cluster = current_cluster_id;
        v_points[i].is_visited = true;
        queue<int> queue_index_vecinos;

        for (int j = 0; j < vecinos_indices.size(); j++) {
          if (!v_points[vecinos_indices[j]].is_visited) {
            queue_index_vecinos.push(vecinos_indices[j]);
            v_points[vecinos_indices[j]].is_visited = true;
          }
        }

        while (!queue_index_vecinos.empty()) {
          int idx = queue_index_vecinos.front();
          queue_index_vecinos.pop();

          vector<int> vecinos_indices = encontrar_vecinos(idx);
          v_points[idx].num_cluster = current_cluster_id;
          if (vecinos_indices.size() >= min_ptrs) {
            v_points[idx].type = 1;

            for (int vecino_idx : vecinos_indices) {
              if (!v_points[vecino_idx].is_visited) {
                queue_index_vecinos.push(vecino_idx);
                v_points[vecino_idx].is_visited = true;
              }
            }
          } else {
            v_points[idx].type = 2;
          }
        }
      }
    }

    for (int i = 0; i < v_points.size(); i++) {
      if (!v_points[i].is_visited) {
        v_points[i].type = 3;         // Ruido
        v_points[i].num_cluster = -1; // Sin cluster
      }
    }
  }
};

vector<vector<double>> leerCSV(const string &filename) {
  vector<vector<double>> datos;
  ifstream archivo(filename);
  string linea;

  if (!archivo.is_open()) {
    cerr << "Error: No se pudo abrir " << filename << endl;
    return datos;
  }

  bool primera_linea = true;

  while (getline(archivo, linea)) {
    if (primera_linea) {
      primera_linea = false;
      continue;
    }

    stringstream ss(linea);
    string valor;
    vector<double> fila;

    while (getline(ss, valor, ',')) {
      try {
        fila.push_back(stod(valor));
      } catch (const exception &e) {
        cerr << "Error convirtiendo valor: " << valor << endl;
      }
    }

    if (!fila.empty()) {
      datos.push_back(fila);
    }
  }

  archivo.close();
  return datos;
}

void escribirCSVConClusters(const string &filename,
                            const string &nombre_archivo_original,
                            const vector<vector<double>> &datos_originales,
                            const vector<Point> &puntos) {
  ofstream archivo(filename);

  if (!archivo.is_open()) {
    cerr << "Error: No se pudo crear " << filename << endl;
    return;
  }

  // Escribir headers CORREGIDOS - "archivo" como columna
  archivo << "archivo,";
  for (size_t i = 0; i < datos_originales[0].size(); i++) {
    archivo << "Dimension_" << i + 1;
    if (i < datos_originales[0].size() - 1)
      archivo << ",";
  }
  archivo << ",Cluster" << endl;

  // Escribir datos con clusters - cada fila tiene el nombre del archivo
  for (size_t i = 0; i < datos_originales.size(); i++) {
    archivo << nombre_archivo_original
            << ","; // Nombre del archivo en cada fila
    for (size_t j = 0; j < datos_originales[i].size(); j++) {
      archivo << datos_originales[i][j];
      if (j < datos_originales[i].size() - 1)
        archivo << ",";
    }
    archivo << "," << puntos[i].num_cluster << endl;
  }

  archivo.close();
  cout << "Archivo de clusters guardado: " << filename << endl;
}

void escribirCSVTiempoEjecucion(const string &filename,
                                const string &nombre_archivo_original,
                                long long tiempo_ejecucion_ms,
                                bool usar_kdtree) {
  ofstream archivo(filename);

  if (!archivo.is_open()) {
    cerr << "Error: No se pudo crear " << filename << endl;
    return;
  }

  // Escribir headers para el archivo de tiempos
  archivo << "archivo,algoritmo,tiempo_ejecucion_ms" << endl;

  // Escribir datos de tiempo de ejecución
  string algoritmo = usar_kdtree ? "DBSCAN_KDTree" : "DBSCAN";
  archivo << nombre_archivo_original << "," << algoritmo << ","
          << tiempo_ejecucion_ms << endl;

  archivo.close();
  cout << "Archivo de tiempos guardado: " << filename << endl;
}

void procesarUnArchivoCSV(const string &archivo_entrada, double epsilon,
                          int min_ptrs, bool usar_kdtree) {
  // Obtener nombre del archivo original sin extensión
  filesystem::path path_entrada(archivo_entrada);
  string nombre_archivo_original = path_entrada.filename().string();
  string nombre_sin_extension = path_entrada.stem().string();

  // Crear carpeta de salida según el método usado
  string carpeta_salida =
      path_entrada.parent_path().string() +
      (usar_kdtree ? "/resultados_dbscan_kdtree/" : "/resultados_dbscan/");
  filesystem::create_directories(carpeta_salida);

  // Nombres de archivos de salida
  string archivo_salida_clusters =
      carpeta_salida + nombre_sin_extension + "_dbscan.csv";
  string archivo_salida_tiempos =
      carpeta_salida + "tiempo_ejecucion_" + nombre_archivo_original;

  cout << "Procesando: " << archivo_entrada << " con "
       << (usar_kdtree ? "KD-Tree" : "búsqueda lineal") << endl;

  // Leer datos del CSV
  vector<vector<double>> datos = leerCSV(archivo_entrada);

  if (datos.empty()) {
    cerr << "  - Archivo vacío o con errores, saltando..." << endl;
    return;
  }

  cout << "  - Puntos leídos: " << datos.size() << endl;
  cout << "  - Dimensiones: " << datos[0].size() << endl;

  // Crear DBSCAN según el método elegido
  CDBScan *dbscan;
  if (usar_kdtree) {
    dbscan = new CDBScan(epsilon, min_ptrs, datos);
  } else {
    dbscan = new CDBScan(epsilon, min_ptrs);
    for (const auto &fila : datos) {
      dbscan->ins(fila);
    }
  }

  // Medir tiempo de ejecución
  auto inicio = chrono::high_resolution_clock::now();
  dbscan->runScan();
  auto fin = chrono::high_resolution_clock::now();
  auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);
  long long tiempo_ms = duracion.count();

  cout << "  - Tiempo de ejecución: " << tiempo_ms << " ms" << endl;

  // Escribir archivo de clusters
  escribirCSVConClusters(archivo_salida_clusters, nombre_archivo_original,
                         datos, dbscan->v_points);

  // Escribir archivo de tiempos de ejecución
  escribirCSVTiempoEjecucion(archivo_salida_tiempos, nombre_archivo_original,
                             tiempo_ms, usar_kdtree);

  cout << "  - Procesado exitosamente: " << archivo_salida_clusters << endl;

  delete dbscan;
}

void procesarArchivosCSV(const string &ruta, double epsilon, int min_ptrs,
                         bool usar_kdtree = false) {
  filesystem::path path(ruta);

  if (filesystem::is_directory(path)) {
    // Si es una carpeta, procesar todos los archivos CSV
    cout << "Procesando carpeta: " << ruta << " con "
         << (usar_kdtree ? "KD-Tree" : "búsqueda lineal") << endl;
    for (const auto &entry : filesystem::directory_iterator(ruta)) {
      if (entry.path().extension() == ".csv") {
        string archivo_entrada = entry.path().string();
        string nombre_archivo = entry.path().filename().string();

        // No procesar archivos que ya son resultados
        if (nombre_archivo.find("_dbscan") != string::npos ||
            nombre_archivo.find("tiempo_ejecucion_") != string::npos) {
          continue;
        }

        procesarUnArchivoCSV(archivo_entrada, epsilon, min_ptrs, usar_kdtree);
      }
    }
  } else if (filesystem::is_regular_file(path) && path.extension() == ".csv") {
    // Si es un archivo CSV individual, procesarlo
    procesarUnArchivoCSV(ruta, epsilon, min_ptrs, usar_kdtree);
  } else {
    cerr << "Error: La ruta no es un archivo CSV válido ni una carpeta: "
         << ruta << endl;
  }
}

int main() {

  bool usar_kdtree = false; // Cambiar a false para usar búsqueda lineal

  if (usar_kdtree) {
    cout << "=== EJECUTANDO CON KD-TREE ===" << endl;
  } else {
    cout << "=== EJECUTANDO CON BÚSQUEDA LINEAL ===" << endl;
  }

  procesarArchivosCSV("./data/hiperesferas_4d.csv", 1.5, 20, usar_kdtree);
  procesarArchivosCSV("./data/hiperesferas_4d_modificado.csv", 0.5, 5,
                      usar_kdtree);
  procesarArchivosCSV("./data/anillo_centro_4d.csv", 1, 10, usar_kdtree);
  procesarArchivosCSV("./data/puentes_4d.csv", 1, 10, usar_kdtree);
  procesarArchivosCSV("./data/circulos_3d.csv", 0.25, 10, usar_kdtree);
  procesarArchivosCSV("./data/densidad_variable_3d.csv", 1.7, 4, usar_kdtree);
  procesarArchivosCSV("./data/espiral_3d.csv", 0.5, 10, usar_kdtree);
  procesarArchivosCSV("./data/lunas_3d.csv", 0.21, 10, usar_kdtree);

  return 0;
}