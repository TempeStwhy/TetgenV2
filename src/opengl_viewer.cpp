#include <GL/glut.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

struct Point3 {
  float x;
  float y;
  float z;
};

struct Edge {
  int a;
  int b;
};

namespace {

std::vector<std::string> tokenize(std::string line) {
  const auto comment = line.find('$');
  if (comment != std::string::npos) {
    line = line.substr(0, comment);
  }
  for (char& c : line) {
    if (c == ',') {
      c = ' ';
    }
  }
  std::stringstream ss(line);
  std::vector<std::string> tokens;
  std::string token;
  while (ss >> token) {
    tokens.push_back(token);
  }
  return tokens;
}

std::pair<int, int> norm_edge(int a, int b) {
  return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
}

std::vector<Point3> g_points;
std::vector<Edge> g_edges;
float g_rot_x = 25.0F;
float g_rot_y = -35.0F;
float g_zoom = 1.0F;
Point3 g_center{0.0F, 0.0F, 0.0F};
float g_radius = 1.0F;

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0F, 0.0F, -3.0F * g_zoom);
  glRotatef(g_rot_x, 1.0F, 0.0F, 0.0F);
  glRotatef(g_rot_y, 0.0F, 1.0F, 0.0F);
  glTranslatef(-g_center.x, -g_center.y, -g_center.z);

  glColor3f(0.95F, 0.95F, 0.95F);
  glBegin(GL_LINES);
  for (const auto& e : g_edges) {
    const auto& a = g_points[e.a];
    const auto& b = g_points[e.b];
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
  }
  glEnd();

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  const float aspect = (h > 0) ? static_cast<float>(w) / static_cast<float>(h) : 1.0F;
  gluPerspective(50.0, aspect, 0.01 * g_radius, 100.0 * g_radius);
}

void keyboard(unsigned char key, int, int) {
  switch (key) {
    case 27:  // ESC
    case 'q':
    case 'Q':
      std::exit(0);
    case '+':
    case '=':
      g_zoom *= 0.9F;
      break;
    case '-':
    case '_':
      g_zoom *= 1.1F;
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT:
      g_rot_y -= 5.0F;
      break;
    case GLUT_KEY_RIGHT:
      g_rot_y += 5.0F;
      break;
    case GLUT_KEY_UP:
      g_rot_x -= 5.0F;
      break;
    case GLUT_KEY_DOWN:
      g_rot_x += 5.0F;
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

bool load_nas_tets(const std::string& path) {
  std::ifstream in(path);
  if (!in) {
    std::cerr << "Failed to open file: " << path << '\n';
    return false;
  }

  std::unordered_map<int, int> id_to_idx;
  std::vector<std::array<int, 4>> tets;

  std::string line;
  while (std::getline(in, line)) {
    auto tokens = tokenize(line);
    if (tokens.empty()) {
      continue;
    }

    if (tokens[0] == "GRID") {
      if (tokens.size() < 5) {
        continue;
      }
      int id = std::stoi(tokens[1]);
      float x = std::stof(tokens[tokens.size() - 3]);
      float y = std::stof(tokens[tokens.size() - 2]);
      float z = std::stof(tokens[tokens.size() - 1]);
      id_to_idx[id] = static_cast<int>(g_points.size());
      g_points.push_back({x, y, z});
    } else if (tokens[0] == "CTETRA") {
      if (tokens.size() < 7) {
        continue;
      }
      tets.push_back({
          std::stoi(tokens[tokens.size() - 4]),
          std::stoi(tokens[tokens.size() - 3]),
          std::stoi(tokens[tokens.size() - 2]),
          std::stoi(tokens[tokens.size() - 1]),
      });
    }
  }

  if (g_points.empty() || tets.empty()) {
    std::cerr << "No GRID/CTETRA found. Please provide a tetrahedral NAS file.\n";
    return false;
  }

  std::unordered_map<long long, bool> edge_seen;
  auto add_edge = [&](int na, int nb) {
    auto [a, b] = norm_edge(na, nb);
    if (id_to_idx.find(a) == id_to_idx.end() || id_to_idx.find(b) == id_to_idx.end()) {
      return;
    }
    int ia = id_to_idx[a];
    int ib = id_to_idx[b];
    auto [u, v] = norm_edge(ia, ib);
    long long key = (static_cast<long long>(u) << 32) | static_cast<unsigned int>(v);
    if (!edge_seen[key]) {
      edge_seen[key] = true;
      g_edges.push_back({u, v});
    }
  };

  for (const auto& t : tets) {
    add_edge(t[0], t[1]);
    add_edge(t[0], t[2]);
    add_edge(t[0], t[3]);
    add_edge(t[1], t[2]);
    add_edge(t[1], t[3]);
    add_edge(t[2], t[3]);
  }

  float minx = g_points[0].x;
  float maxx = g_points[0].x;
  float miny = g_points[0].y;
  float maxy = g_points[0].y;
  float minz = g_points[0].z;
  float maxz = g_points[0].z;
  for (const auto& p : g_points) {
    minx = std::min(minx, p.x);
    maxx = std::max(maxx, p.x);
    miny = std::min(miny, p.y);
    maxy = std::max(maxy, p.y);
    minz = std::min(minz, p.z);
    maxz = std::max(maxz, p.z);
  }
  g_center = {(minx + maxx) * 0.5F, (miny + maxy) * 0.5F, (minz + maxz) * 0.5F};
  const float dx = maxx - minx;
  const float dy = maxy - miny;
  const float dz = maxz - minz;
  g_radius = std::sqrt(dx * dx + dy * dy + dz * dz) * 0.5F;
  if (g_radius < 1e-5F) {
    g_radius = 1.0F;
  }

  return true;
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Usage: tet_view_gl <tet_mesh.nas>\n";
    return 1;
  }

  if (!load_nas_tets(argv[1])) {
    return 2;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1280, 800);
  glutCreateWindow("Tet Mesh OpenGL Viewer");

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.12F, 0.14F, 0.18F, 1.0F);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);

  std::cout << "Controls: Arrow keys rotate, +/- zoom, q or ESC exits.\n";
  glutMainLoop();
  return 0;
}
