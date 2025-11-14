#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

struct Node {
  vector<double> point;
  Node *left;
  Node *right;
  Node(const vector<double> &p) {
    point = p;
    left = nullptr;
    right = nullptr;
  }
};

class KDTree {
private:
  Node *root;
  int k;

  Node *insertRec(Node *node, const vector<double> &point, int depth) {
    if (node == nullptr) {
      return new Node(point);
    }
    int axis = depth % k;
    if (point[axis] < node->point[axis]) {
      node->left = insertRec(node->left, point, depth + 1);
    } else {
      node->right = insertRec(node->right, point, depth + 1);
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
                  vector<double> &best, double &bestDist) {
    if (!node) {
      return;
    }

    double d = dist2(target, node->point);

    if (d < bestDist) {
      bestDist = d;
      best = node->point;
    }
    int axis = depth % k;
    Node *next = (target[axis] < node->point[axis]) ? node->left : node->right;
    Node *other = (next == node->left) ? node->right : node->left;
    nearestRec(next, target, depth + 1, best, bestDist);

    if (fabs(target[axis] - node->point[axis]) < sqrt(bestDist)) {
      nearestRec(other, target, depth + 1, best, bestDist);
    }
  }

public:
  KDTree(int dimensions) {
    root = nullptr;
    k = dimensions;
  }

  void insert(const vector<double> &point) { root = insertRec(root, point, 0); }

  vector<double> nearest(const vector<double> &target) {
    vector<double> best(k);
    double bestDist = numeric_limits<double>::max();
    nearestRec(root, target, 0, best, bestDist);
    return best;
  }
};

vector<vector<double>> leerCSV(const string &filename, int n) {
  vector<vector<double>> puntos;
  ifstream file(filename);
  string line;
  int count = 0;

  if (!file.is_open()) {
    cerr << "No se pudo abrir el archivo: " << filename << "\n";
    return puntos;
  }

  while (getline(file, line) && count < n) {

    if (line.empty())
      continue;
    for (auto &ch : line) {
      if (ch == '\r' || ch == '\n')
        ch = ' ';
    }

    stringstream ss(line);
    string val;
    vector<double> p;
    while (getline(ss, val, ',')) {

      size_t start = val.find_first_not_of(" \t");
      size_t end = val.find_last_not_of(" \t");
      if (start == string::npos || end == string::npos)
        continue;
      val = val.substr(start, end - start + 1);

      try {
        p.push_back(stod(val));
      } catch (...) {

        p.clear();
        break;
      }
    }

    if (p.size() >= 2) {
      puntos.push_back({p[0], p[1]});
      count++;
    }
  }

  file.close();
  return puntos;
}

vector<vector<double>> kmeans_kdtree(vector<vector<double>> &puntos, int k,
                                     vector<int> &clusterAsign,
                                     double &tiempo) {
  int n = puntos.size();
  vector<vector<double>> centroides(k);

  for (int i = 0; i < k; i++)
    centroides[i] = puntos[rand() % n];

  auto start = chrono::high_resolution_clock::now();

  bool cambio = true;
  int iteraciones = 0;

  while (cambio) {
    cambio = false;
    iteraciones++;

    KDTree tree(2);
    for (auto &c : centroides)
      tree.insert(c);

    for (int i = 0; i < n; i++) {
      vector<double> cerca = tree.nearest(puntos[i]);

      for (int j = 0; j < k; j++) {
        if (centroides[j] == cerca) {
          if (clusterAsign[i] != j) {
            clusterAsign[i] = j;
            cambio = true;
          }
          break;
        }
      }
    }

    vector<vector<double>> nuevos(k, vector<double>(2, 0));
    vector<int> cuenta(k, 0);

    for (int i = 0; i < n; i++) {
      int c = clusterAsign[i];
      cuenta[c]++;
      nuevos[c][0] += puntos[i][0];
      nuevos[c][1] += puntos[i][1];
    }

    for (int j = 0; j < k; j++) {
      if (cuenta[j] > 0) {
        nuevos[j][0] /= cuenta[j];
        nuevos[j][1] /= cuenta[j];
      }
    }

    double tolerancia = 1e-6;
    for (int j = 0; j < k; j++) {
      if (fabs(nuevos[j][0] - centroides[j][0]) > tolerancia ||
          fabs(nuevos[j][1] - centroides[j][1]) > tolerancia) {
        cambio = true;
        break;
      }
    }

    centroides = nuevos;
  }

  auto end = chrono::high_resolution_clock::now();
  tiempo = chrono::duration<double, milli>(end - start).count();

  cout << " Iteraciones hasta estabilizar: " << iteraciones << "\n";
  return centroides;
}

// =================== MAIN ===================

// OG
/*
int main()
{
    srand((unsigned)time(NULL));

    string archivo = "C:\\Users\\USER\\Downloads\\data2k.csv";


    int n = 2400;
    int k = 5;



    cout << "Archivo cargado: " << archivo << "\n";
    cout << "Cantidad de puntos: " << n << "\n";
    cout << "Cantidad de clusters: " << k << "\n\n";

    // 🔹 Leer puntos desde el CSV
    vector<vector<double>> puntos = leerCSV(archivo, n);
    if (puntos.empty())
    {
        cerr << "Error\n";
        return 1;
    }

    vector<int> cluster(n);
    double t_kd;

    // 🔹 Ejecutar K-Means con KD-Tree
    vector<vector<double>> c2 = kmeans_kdtree(puntos, k, cluster, t_kd);

    // 🔹 Guardar resultados
    ofstream tiempos("tiemposJ.csv");
    tiempos << "version,n_puntos,n_clusters,tiempo_ms\n";
    tiempos << "KDTree," << n << "," << k << "," << t_kd << "\n";
    tiempos.close();


    ofstream clusters("clusters10.csv");
    clusters << "x,y,cluster,centro_x,centro_y\n";
    for (int i = 0; i < n; i++) {
        int c = cluster[i];
        clusters << puntos[i][0] << "," << puntos[i][1] << "," << c << ","
            << c2[c][0] << "," << c2[c][1] << "\n";
    }
    clusters.close();


    cout << "Archivos generados correctamente:\n";


    return 0;
}
*/

// C
/*
int main() {
    srand((unsigned)time(NULL));

    string archivo = "C:\\Users\\USER\\Downloads\\data2k.csv";

    // Valores de n (puntos)
    vector<int> valoresN = { 1000, 1150, 1300, 1450, 1600, 1750, 1900, 2050,
2200, 2400 };

    // Valores de k (clusters)
    vector<int> valoresK = { 5, 15, 25, 50, 75 };




    for (size_t idx = 0; idx < valoresK.size(); ++idx) {
        int k = valoresK[idx];
        string nombreCSV = "tiempos__" + to_string(idx + 1) + ".csv";

        ofstream tiempos(nombreCSV);
        tiempos << "version,n_puntos,n_clusters,tiempo_ms\n";

        cout << "\n=== Ejecutando para k = " << k << " ===\n";


        for (int n : valoresN) {
            cout << " -> n = " << n << endl;

            vector<vector<double>> puntos = leerCSV(archivo, n);
            if (puntos.empty()) {
                cerr << "Error al leer puntos del CSV\n";
                return 1;
            }

            vector<int> cluster(n);
            double t_kd = 0.0;

            vector<vector<double>> c2 = kmeans_kdtree(puntos, k, cluster, t_kd);


            tiempos << "KDTree," << n << "," << k << "," << t_kd << "\n";
        }

        tiempos.close();
        cout << "Archivo generado: " << nombreCSV << "\n";
    }


    return 0;
}
*/

int main() {
  srand((unsigned)time(NULL));

  string archivo = "C:\\Users\\USER\\Downloads\\data2k.csv";

  vector<int> valoresN = {1000, 1450, 1900, 2400};

  vector<int> valoresK = {5, 15, 25, 50, 75, 100, 125, 150, 200};

  for (int n : valoresN) {
    string nombreCSV = "tiempos_n_" + to_string(n) + ".csv";
    ofstream tiempos(nombreCSV);
    tiempos << "version,n_puntos,n_clusters,tiempo_ms\n";

    cout << "\n=== Ejecutando para n = " << n << " ===\n";

    vector<vector<double>> puntos = leerCSV(archivo, n);
    if (puntos.empty()) {
      cerr << "Error al leer puntos del CSV\n";
      return 1;
    }

    for (int k : valoresK) {
      cout << " -> k = " << k << endl;

      vector<int> cluster(n);
      double t_kd = 0.0;

      vector<vector<double>> c2 = kmeans_kdtree(puntos, k, cluster, t_kd);

      tiempos << "KDTree," << n << "," << k << "," << t_kd << "\n";
    }

    tiempos.close();
    cout << "Archivo generado: " << nombreCSV << "\n";
  }

  return 0;
}