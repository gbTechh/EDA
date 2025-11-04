#include "RTree.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void save_step(int step, const vector<pair<int, int>> &rectPoints, RTree &rtree,
               const string &folder) {
  vector<vector<vector<pair<int, int>>>> objects_n;
  rtree.getMBRs(objects_n);

  Rect rect = rtree.MBR(const_cast<vector<pair<int, int>> &>(rectPoints));

  stringstream fname;
  fname << folder << "/step_" << step << ".json";
  ofstream out(fname.str());

  out << "{\n";
  out << "  \"all_points\": [";
  for (size_t i = 0; i < rtree.mObjs.size(); i++) {
    for (size_t j = 0; j < rtree.mObjs[i].size(); j++) {
      out << "[" << rtree.mObjs[i][j].first << "," << rtree.mObjs[i][j].second
          << "]";
      if (!(i == rtree.mObjs.size() - 1 && j == rtree.mObjs[i].size() - 1))
        out << ",";
    }
  }
  out << "],\n";

  out << "  \"mbr\": [" << rect.m_min[0] << "," << rect.m_min[1] << ","
      << rect.m_max[0] << "," << rect.m_max[1] << "],\n";

  out << "  \"tree\": [\n";
  for (size_t lvl = 0; lvl < objects_n.size(); lvl++) {
    out << "    [";
    for (size_t j = 0; j < objects_n[lvl].size(); j++) {
      auto &r = objects_n[lvl][j];
      int xmin = r[0].first, ymin = r[0].second;
      int xmax = r[1].first, ymax = r[1].second;
      out << "[[" << xmin << "," << ymin << "],[" << xmax << "," << ymax
          << "]]";
      if (j < objects_n[lvl].size() - 1)
        out << ",";
    }
    out << "]";
    if (lvl < objects_n.size() - 1)
      out << ",";
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";

  out.close();
}

int main() {
  RTree rtree;

  string folder = "outputs/MAXNODES_2_FIXED";
  system(("mkdir -p " + folder).c_str());

  // Rectángulos a insertar (minX, minY, maxX, maxY)
  int rects[6][4] = {
      {20, 43, 20, 59},  // R1
      {48, 58, 50, 67},  // R2
      {74, 64, 105, 68}, // R3
      {83, 40, 104, 54}, // R4
      {20, 43, 20, 59},  // R5 (igual que R1)
      {48, 67, 105, 68}  // R6
  };

  for (int i = 0; i < 6; i++) {
    vector<pair<int, int>> data;
    data.push_back({rects[i][0], rects[i][1]});
    data.push_back({rects[i][2], rects[i][3]});

    Rect rect(rects[i][0], rects[i][1], rects[i][2], rects[i][3]);
    rtree.Insert(rect.m_min, rect.m_max, data);

    save_step(i + 1, data, rtree, folder);
    cout << "Rectangulo " << i + 1 << " insertado" << endl;
  }

  Rect query(0, 0, 40, 50);
  vector<vector<pair<int, int>>> results;
  rtree.Search(rtree.m_root, &query, results);

  cout << "Resultados de Search en (0,0)-(25,25):\n";
  if (results.empty()) {
    cout << "  No se encontraron rectángulos.\n";
  } else {
    for (size_t i = 0; i < results.size(); i++) {
      cout << "  Objeto " << i + 1 << ": ";
      for (auto &p : results[i]) {
        cout << "(" << p.first << "," << p.second << ") ";
      }
      cout << "\n";
    }
  }

  return 0;
}
