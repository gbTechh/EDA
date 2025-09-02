#ifndef OCTREE_H
#define OCTREE_H

#include <algorithm>
#include <limits>
#include <vector>

struct Point {
  float x;
  float y;
  float z;

  Point(float a, float b, float c) : x(a), y(b), z(c) {}
};

class Octree {
private:
  static const int MAX_CAPACITY = 8;
  Octree *children[8];
  std::vector<Point> points; // std::vector<Point> points
  std::vector<Point> v_points;
  // bottomLeft y h definen el espacio(cubo más grande)
  Point bottomLeft;
  float h;

  int nPoints; // puntos ingresados.

  float max_x = std::numeric_limits<float>::lowest();
  float max_y = std::numeric_limits<float>::lowest();
  float max_z = std::numeric_limits<float>::lowest();
  float min_x = std::numeric_limits<float>::max();
  float min_y = std::numeric_limits<float>::max();
  float min_z = std::numeric_limits<float>::max();

private:
  bool isInNode(const Point p);
  int getOctante(const Point &p);
  void findClosestHelper(const Point &target, float radius, Point &closest,
                         float &min_distance, bool &found);
  float calculateDistance(const Point &p1, const Point &p2);

public:
  Octree();
  Octree(Point bl, float size);
  void readPoints(float a, float b, float c);
  bool exist(const Point &);
  void insert(const Point &);
  Point find_closest(const Point &, int radius);
  void printTree(int depth = 0);

  void inserAll();
  float getH() {
    float x = max_x - min_x;
    float y = max_y - min_y;
    float z = max_z - min_z;

    h = std::max({x, y, z});
    return h;
  };
  Point getBottomLeft() {
    bottomLeft = Point(min_x, min_y, min_z);
    return bottomLeft;
  };
  std::vector<Point> getVPoints() { return v_points; }
};

#endif
