#include "nastran_io.h"

#include "tetgen.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

tetgenio build_tetgen_input(const SurfaceMesh& surf) {
  tetgenio in;
  in.firstnumber = 0;
  in.numberofpoints = static_cast<int>(surf.points.size());
  in.pointlist = new REAL[in.numberofpoints * 3];

  for (int i = 0; i < in.numberofpoints; ++i) {
    in.pointlist[i * 3 + 0] = surf.points[i].x;
    in.pointlist[i * 3 + 1] = surf.points[i].y;
    in.pointlist[i * 3 + 2] = surf.points[i].z;
  }

  in.numberoffacets = static_cast<int>(surf.triangles.size());
  in.facetlist = new tetgenio::facet[in.numberoffacets];

  for (int i = 0; i < in.numberoffacets; ++i) {
    tetgenio::facet& facet = in.facetlist[i];
    facet.numberofpolygons = 1;
    facet.polygonlist = new tetgenio::polygon[1];
    facet.numberofholes = 0;
    facet.holelist = nullptr;

    tetgenio::polygon& polygon = facet.polygonlist[0];
    polygon.numberofvertices = 3;
    polygon.vertexlist = new int[3];
    polygon.vertexlist[0] = surf.triangles[i].v[0];
    polygon.vertexlist[1] = surf.triangles[i].v[1];
    polygon.vertexlist[2] = surf.triangles[i].v[2];
  }

  return in;
}

TetMesh build_output_mesh(const tetgenio& out) {
  if (out.numberofpoints <= 0 || out.numberoftetrahedra <= 0) {
    throw std::runtime_error("TetGen did not generate a valid tetrahedral mesh. Check if the input is closed and non-self-intersecting.");
  }

  TetMesh mesh;
  mesh.points.reserve(static_cast<size_t>(out.numberofpoints));
  for (int i = 0; i < out.numberofpoints; ++i) {
    mesh.points.push_back({out.pointlist[i * 3 + 0], out.pointlist[i * 3 + 1], out.pointlist[i * 3 + 2]});
  }

  mesh.tets.reserve(static_cast<size_t>(out.numberoftetrahedra));
  for (int i = 0; i < out.numberoftetrahedra; ++i) {
    mesh.tets.push_back({{
        out.tetrahedronlist[i * out.numberofcorners + 0],
        out.tetrahedronlist[i * out.numberofcorners + 1],
        out.tetrahedronlist[i * out.numberofcorners + 2],
        out.tetrahedronlist[i * out.numberofcorners + 3],
    }});
  }

  if (out.numberoftrifaces > 0 && out.trifacelist != nullptr) {
    mesh.boundary_triangles.reserve(static_cast<size_t>(out.numberoftrifaces));
    for (int i = 0; i < out.numberoftrifaces; ++i) {
      mesh.boundary_triangles.push_back({{
          out.trifacelist[i * 3 + 0],
          out.trifacelist[i * 3 + 1],
          out.trifacelist[i * 3 + 2],
      }});
    }
  }

  return mesh;
}

void print_usage() {
  std::cout << "Usage:\n"
            << "  nas2tet <input.nas> <output_tet.nas> [output.vtu] [tetgen_switches]\n\n"
            << "Example:\n"
            << "  nas2tet model.nas model_tet.nas model.vtu pq1.2a0.01\n";
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    print_usage();
    return 1;
  }

  const std::string input_path = argv[1];
  const std::string output_nas_path = argv[2];
  const std::string output_vtu_path = (argc >= 4) ? argv[3] : output_nas_path + ".vtu";
  std::string switches = (argc >= 5) ? argv[4] : "pq1.2aA";

  try {
    const SurfaceMesh surface = read_nastran_surface(input_path);

    tetgenio in = build_tetgen_input(surface);
    tetgenio out;

    tetrahedralize(switches.data(), &in, &out);

    const TetMesh mesh = build_output_mesh(out);
    write_nastran_tets(output_nas_path, mesh);
    write_vtu(output_vtu_path, mesh);

    std::cout << "Conversion completed.\n"
              << "  Input surface: " << input_path << "\n"
              << "  Output tet mesh (NAS): " << output_nas_path << "\n"
              << "  Visualization file (VTU): " << output_vtu_path << "\n"
              << "  Number of points: " << mesh.points.size() << "\n"
              << "  Number of tets: " << mesh.tets.size() << "\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 2;
  }

  return 0;
}
