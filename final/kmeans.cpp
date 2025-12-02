#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

using namespace std;

struct Point {
  double x;
  double y;
};

class KMeans {
public:
  int k;
  vector<Point> v_puntos;
  vector<Point> k_center;
  vector<vector<Point>> v_clustering;

public:
  KMeans(int _k, vector<Point> data) {
    k = _k;
    v_puntos = data;

    k_center.resize(k);
    v_clustering.resize(k);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, data.size() - 1);

    for (int i = 0; i < k; i++) {
      int num = distrib(gen);
      k_center[i] = data[num];
    }
  }

  void entrenar(int max_it = 100) {
    for (int i = 0; i < max_it; i++) {
      vector<Point> centroides_anteriores = k_center;
      for (auto &cluster : v_clustering) {
        cluster.clear();
      }
      pointCenter();
      recalcularCenter();

      if (convergen(centroides_anteriores)) {
        break;
      }
    }
  }

  double checkDistance(Point p1, Point p2) {
    double x = p1.x - p2.x;
    double y = p1.y - p2.y;
    return sqrt(x * x + y * y);
  }

  void pointCenter() {
    Point p;
    for (int i = 0; i < v_puntos.size(); i++) {
      double distance = std::numeric_limits<double>::max();
      int index = 0;
      for (int j = 0; j < k_center.size(); j++) {
        double dis = checkDistance(v_puntos[i], k_center[j]);
        if (dis < distance) {
          distance = dis;
          index = j;
        }
      }
      v_clustering[index].push_back(v_puntos[i]);
    }
  }

  Point calculateMedia(vector<Point> v_puntos) {
    Point p;
    double x, y = 0.0;
    for (int i = 0; i < v_puntos.size(); i++) {
      x += v_puntos[i].x;
      y += v_puntos[i].y;
    }
    p.x = x / v_puntos.size();
    p.y = y / v_puntos.size();
    return p;
  }

  void recalcularCenter() {
    for (int i = 0; i < k_center.size(); i++) {
      if (v_clustering[i].empty()) {
        continue;
      }
      k_center[i] = calculateMedia(v_clustering[i]);
    }
  }

  bool convergen(const vector<Point> prev_puntos, double ep = 1e-10) {
    for (int i = 0; i < k; i++) {
      double x = prev_puntos[i].x - k_center[i].x;
      double y = prev_puntos[i].y - k_center[i].y;
      double dis = sqrt(x * x + y * y);
      if (dis > ep) {
        return false;
      }
    }

    return true;
  }
};

int main() {}