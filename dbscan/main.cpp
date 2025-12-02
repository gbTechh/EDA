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

class CDBScan {
public:
  int dim;
  double epsilon;
  int min_ptrs;
  vector<Point> v_points;
  unordered_map<int, vector<Point>> m_cluster;

public:
  CDBScan(double ep, int min) : epsilon(ep), min_ptrs(min) {}

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

  vector<int> encontrar_vecinos(int index) {
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
                                long long tiempo_ejecucion_ms) {
  ofstream archivo(filename);

  if (!archivo.is_open()) {
    cerr << "Error: No se pudo crear " << filename << endl;
    return;
  }

  // Escribir headers para el archivo de tiempos
  archivo << "archivo,algoritmo,tiempo_ejecucion_ms" << endl;

  // Escribir datos de tiempo de ejecución
  archivo << nombre_archivo_original << ",DBSCAN," << tiempo_ejecucion_ms
          << endl;

  archivo.close();
  cout << "Archivo de tiempos guardado: " << filename << endl;
}

void procesarUnArchivoCSV(const string &archivo_entrada, double epsilon,
                          int min_ptrs) {
  // Obtener nombre del archivo original sin extensión
  filesystem::path path_entrada(archivo_entrada);
  string nombre_archivo_original = path_entrada.filename().string();
  string nombre_sin_extension = path_entrada.stem().string();

  // Crear carpeta de salida si no existe
  string carpeta_salida =
      path_entrada.parent_path().string() + "/resultados_dbscan/";
  filesystem::create_directories(carpeta_salida);

  // Nombres de archivos de salida CORREGIDOS
  string archivo_salida_clusters =
      carpeta_salida + nombre_sin_extension + "_dbscan.csv";
  string archivo_salida_tiempos =
      carpeta_salida + "tiempo_ejecucion_" + nombre_archivo_original;

  cout << "Procesando: " << archivo_entrada << endl;

  // Leer datos del CSV
  vector<vector<double>> datos = leerCSV(archivo_entrada);

  if (datos.empty()) {
    cerr << "  - Archivo vacío o con errores, saltando..." << endl;
    return;
  }

  cout << "  - Puntos leídos: " << datos.size() << endl;
  cout << "  - Dimensiones: " << datos[0].size() << endl;

  // Crear DBSCAN y medir tiempo de ejecución
  CDBScan dbscan(epsilon, min_ptrs);

  for (const auto &fila : datos) {
    dbscan.ins(fila);
  }

  // Medir tiempo de ejecución
  auto inicio = chrono::high_resolution_clock::now();
  dbscan.runScan();
  auto fin = chrono::high_resolution_clock::now();
  auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);
  long long tiempo_ms = duracion.count();

  cout << "  - Tiempo de ejecución: " << tiempo_ms << " ms" << endl;

  // Escribir archivo de clusters
  escribirCSVConClusters(archivo_salida_clusters, nombre_archivo_original,
                         datos, dbscan.v_points);

  // Escribir archivo de tiempos de ejecución
  escribirCSVTiempoEjecucion(archivo_salida_tiempos, nombre_archivo_original,
                             tiempo_ms);

  cout << "  - Procesado exitosamente: " << archivo_salida_clusters << endl;
}

void procesarArchivosCSV(const string &ruta, double epsilon, int min_ptrs) {
  filesystem::path path(ruta);

  if (filesystem::is_directory(path)) {
    // Si es una carpeta, procesar todos los archivos CSV
    cout << "Procesando carpeta: " << ruta << endl;
    for (const auto &entry : filesystem::directory_iterator(ruta)) {
      if (entry.path().extension() == ".csv") {
        string archivo_entrada = entry.path().string();
        string nombre_archivo = entry.path().filename().string();

        // No procesar archivos que ya son resultados
        if (nombre_archivo.find("_dbscan") != string::npos ||
            nombre_archivo.find("tiempo_ejecucion_") != string::npos) {
          continue;
        }

        procesarUnArchivoCSV(archivo_entrada, epsilon, min_ptrs);
      }
    }
  } else if (filesystem::is_regular_file(path) && path.extension() == ".csv") {
    // Si es un archivo CSV individual, procesarlo
    procesarUnArchivoCSV(ruta, epsilon, min_ptrs);
  } else {
    cerr << "Error: La ruta no es un archivo CSV válido ni una carpeta: "
         << ruta << endl;
  }
}

int main() {
  CDBScan dbscan(0.5, 5);

  // Datos de prueba con vectores
  dbscan.ins({0.0, 0.0});
  dbscan.ins({0.0, 1.0});
  dbscan.ins({1.0, 0.0});
  dbscan.ins({1.0, 1.0});
  dbscan.ins({0.5, 0.5});
  dbscan.ins({5.0, 5.0}); // Ruido

  dbscan.runScan();

  // Mostrar resultados
  cout << "Resultados DBSCAN:" << endl;
  for (size_t i = 0; i < dbscan.v_points.size(); i++) {
    string tipo;
    switch (dbscan.v_points[i].type) {
    case 1:
      tipo = "NUCLEO";
      break;
    case 2:
      tipo = "BORDE";
      break;
    case 3:
      tipo = "RUIDO";
      break;
    default:
      tipo = "DESCONOCIDO";
    }
    cout << "Punto " << i << " [" << dbscan.v_points[i].v_data[0] << ", "
         << dbscan.v_points[i].v_data[1] << "]: Cluster "
         << dbscan.v_points[i].num_cluster << ", Tipo: " << tipo << endl;
  }

  cout << "\nTotal de puntos: " << dbscan.v_points.size() << endl;
  procesarArchivosCSV("./data/hiperesferas_4d.csv", 1.5, 20);
  procesarArchivosCSV("./data/anillo_centro_4d.csv", 1, 10);
  procesarArchivosCSV("./data/puentes_4d.csv", 1, 10);
  procesarArchivosCSV("./data/circulos_3d.csv", 0.25, 10);
  procesarArchivosCSV("./data/densidad_variable_3d.csv", 1.7, 4);
  procesarArchivosCSV("./data/espiral_3d.csv", 0.5, 10);
  procesarArchivosCSV("./data/lunas_3d.csv", 0.21, 10);
  return 0;
}