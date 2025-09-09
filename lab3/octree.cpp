#include "octree.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>

// Constructor que inicializa bottomLeft y h en la lista de inicialización
Octree::Octree() : bottomLeft(0, 0, 0), h(0.0) {
  for (int i = 0; i < 8; i++) {
    children[i] = nullptr;
  }
  nPoints = 0;
  max_x = max_y = max_z = std::numeric_limits<float>::lowest();
  min_x = min_y = min_z = std::numeric_limits<float>::max();
}
Octree::Octree(Point bl, float size) : bottomLeft(bl), h(size) {
  for (int i = 0; i < 8; i++) {
    children[i] = nullptr;
  }
  nPoints = 0;
}

void Octree::readPoints(float x, float y, float z) {
  if (x < min_x)
    min_x = x;
  if (y < min_y)
    min_y = y;
  if (z < min_z)
    min_z = z;
  if (x > max_x)
    max_x = x;
  if (y > max_y)
    max_y = y;
  if (z > max_z)
    max_z = z;

  v_points.push_back({x, y, z});
}

bool Octree::exist(const Point &p) {
  if (!isInNode(p)) {
    return false;
  }

  // Si es un nodo hoja, buscar en el vevctor de puntos del nodo
  if (children[0] == nullptr) {
    for (const Point &punto : points) {
      if (punto.x == p.x && punto.y == p.y && punto.z == p.z) {
        return true;
      }
    }
    return false;
  } else {
    // Si tiene hijos, buscar en el octante correspondiente
    int octante = getOctante(p);
    return children[octante]->exist(p);
  }
}

void Octree::insert(const Point &p) {
  if (!isInNode(p)) {
    return;
  }
  ++nPoints;
  if (children[0] == nullptr) {
    if (points.size() < MAX_CAPACITY) {
      points.push_back(p);
    } else {
      // Subdividir
      float child_h = h / 2.0f;
      // for (int i = 0; i < 8; i++) {
      //   float child_x = bottomLeft.x + (i & 1) * child_h;
      //   float child_y = bottomLeft.y + ((i >> 1) & 1) * child_h;
      //   float child_z = bottomLeft.z + ((i >> 2) & 1) * child_h;
      //   children[i] = new Octree(Point(child_x, child_y, child_z), child_h);
      // }

      // Octante 0: (-x, -y, -z) - esquina inferior izquierda trasera
      children[0] =
          new Octree(Point(bottomLeft.x, bottomLeft.y, bottomLeft.z), child_h);

      // Octante 1: (+x, -y, -z) - esquina inferior derecha trasera
      children[1] = new Octree(
          Point(bottomLeft.x + child_h, bottomLeft.y, bottomLeft.z), child_h);

      // Octante 2: (-x, +y, -z) - esquina superior izquierda trasera
      children[2] = new Octree(
          Point(bottomLeft.x, bottomLeft.y + child_h, bottomLeft.z), child_h);

      // Octante 3: (+x, +y, -z) - esquina superior derecha trasera
      children[3] = new Octree(
          Point(bottomLeft.x + child_h, bottomLeft.y + child_h, bottomLeft.z),
          child_h);

      // Octante 4: (-x, -y, +z) - esquina inferior izquierda frontal
      children[4] = new Octree(
          Point(bottomLeft.x, bottomLeft.y, bottomLeft.z + child_h), child_h);

      // Octante 5: (+x, -y, +z) - esquina inferior derecha frontal
      children[5] = new Octree(
          Point(bottomLeft.x + child_h, bottomLeft.y, bottomLeft.z + child_h),
          child_h);

      // Octante 6: (-x, +y, +z) - esquina superior izquierda frontal
      children[6] = new Octree(
          Point(bottomLeft.x, bottomLeft.y + child_h, bottomLeft.z + child_h),
          child_h);

      // Octante 7: (+x, +y, +z) - esquina superior derecha frontal
      children[7] =
          new Octree(Point(bottomLeft.x + child_h, bottomLeft.y + child_h,
                           bottomLeft.z + child_h),
                     child_h);

      // Redistribuir puntos
      for (const Point &punto : points) {
        int octante = getOctante(punto);
        children[octante]->insert(punto);
      }
      points.clear();

      // Insertar el nuevo punto
      int octante = getOctante(p);
      children[octante]->insert(p);
    }
  } else {
    int octante = getOctante(p);
    children[octante]->insert(p);
  }
}

Point Octree::find_closest(const Point &p, float radius) {
  Point closest(0, 0, 0);
  float min_distance = std::numeric_limits<float>::max();
  bool found = false;

  findClosestHelper(p, radius, closest, min_distance, found);

  if (!found) {
    std::cout << "No se encontró punto cercano en el radio " << radius
              << std::endl;
    return Point(0, 0, 0);
  }

  return closest;
}

void Octree::inserAll() {
  for (Point p : v_points) {
    insert(p);
  }
}

void Octree::printTree(int depth) {
  std::string indent(depth * 2, ' '); // Sangría según profundidad

  std::cout << indent << "Nivel " << depth << ": ";
  std::cout << "[" << bottomLeft.x << ", " << bottomLeft.y << ", "
            << bottomLeft.z << "]";
  std::cout << " h=" << h << std::endl;

  std::cout << indent << "Puntos: " << nPoints << std::endl;
  if (!points.empty()) {
    for (size_t i = 0; i < points.size(); i++) {
      std::cout << indent << "  (" << points[i].x << ", " << points[i].y << ", "
                << points[i].z << ")" << std::endl;
    }
  }

  // Recorrer hijos si existen
  for (int i = 0; i < 8; i++) {
    if (children[i] != nullptr) {
      std::cout << indent << "Hijo " << i << ":" << std::endl;
      children[i]->printTree(depth + 1);
    }
  }
}

bool Octree::isInNode(const Point p) {
  return (bottomLeft.x <= p.x && p.x <= bottomLeft.x + h &&
          bottomLeft.y <= p.y && p.y <= bottomLeft.y + h &&
          bottomLeft.z <= p.z && p.z <= bottomLeft.z + h);
}

int Octree::getOctante(const Point &p) {
  float centro_x = bottomLeft.x + h / 2;
  float centro_y = bottomLeft.y + h / 2;
  float centro_z = bottomLeft.z + h / 2;

  int oct = 0;
  if (p.x > centro_x)
    oct |= 1; // bit 0: X
  if (p.y > centro_y)
    oct |= 2; // bit 1: Y
  if (p.z > centro_z)
    oct |= 4; // bit 2: Z

  return oct;
}

float Octree::calculateDistance(const Point &p1, const Point &p2) {
  float dx = p1.x - p2.x;
  float dy = p1.y - p2.y;
  float dz = p1.z - p2.z;
  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float Octree::distanceToNode(const Point &target) {
  // Calcular el punto más cercano en el cubo al punto target
  float closest_x =
      std::max(bottomLeft.x, std::min(target.x, bottomLeft.x + h));
  float closest_y =
      std::max(bottomLeft.y, std::min(target.y, bottomLeft.y + h));
  float closest_z =
      std::max(bottomLeft.z, std::min(target.z, bottomLeft.z + h));

  // Calcular distancia entre target y el punto más cercano del cubo
  float dx = target.x - closest_x;
  float dy = target.y - closest_y;
  float dz = target.z - closest_z;

  return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void Octree::findClosestHelper(const Point &target, float radius,
                               Point &closest, float &min_distance,
                               bool &found) {
  // 1. Calcular distancia mínima al nodo (CORREGIDO)
  float node_distance = distanceToNode(target);

  // 2. Si el nodo está completamente fuera del radio, salir
  if (node_distance > radius) {
    return;
  }

  // 3. Si es NODO HOJA, revisar sus puntos
  if (children[0] == nullptr) {
    for (const Point &punto : points) {
      float distance = calculateDistance(target, punto);
      if (distance <= radius && distance < min_distance) {
        min_distance = distance;
        closest = punto;
        found = true;
        std::cout << "Nuevo punto mas cercano encontrado: (" << punto.x << ", "
                  << punto.y << ", " << punto.z << ") distancia: " << distance
                  << std::endl;
      }
    }
  }
  // 4. Si es NODO INTERNO, buscar en hijos
  else {
    for (int i = 0; i < 8; i++) {
      if (children[i] != nullptr) {
        children[i]->findClosestHelper(target, radius, closest, min_distance,
                                       found);
      }
    }
  }
}

void Octree::findNodeForPoint(const Point &p, Point &nodeBottomLeft,
                              float &nodeH) {
  // 1. Verificar si el punto está en este nodo
  if (!isInNode(p)) {
    return; // No está aquí
  }

  // 2. Si es nodo hoja, este es el nodo que contiene el punto
  if (children[0] == nullptr) {
    nodeBottomLeft = bottomLeft;
    nodeH = h;
    return;
  }

  // 3. Si es nodo interno, buscar en el hijo correspondiente
  int octante = getOctante(p);
  if (children[octante] != nullptr) {
    children[octante]->findNodeForPoint(p, nodeBottomLeft, nodeH);
  }
}