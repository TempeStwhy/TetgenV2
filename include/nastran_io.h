#pragma once

#include <array>
#include <string>
#include <vector>

struct Point3 {
  double x;
  double y;
  double z;
};

struct TriFace {
  std::array<int, 3> v;
};

struct TetCell {
  std::array<int, 4> v;
};

struct SurfaceMesh {
  std::vector<Point3> points;
  std::vector<TriFace> triangles;
};

struct TetMesh {
  std::vector<Point3> points;
  std::vector<TetCell> tets;
  std::vector<TriFace> boundary_triangles;
};

SurfaceMesh read_nastran_surface(const std::string& path);
void write_nastran_tets(const std::string& path, const TetMesh& mesh);
void write_vtu(const std::string& path, const TetMesh& mesh);
