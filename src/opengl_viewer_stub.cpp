#include <iostream>

int main() {
  std::cerr << "tet_view_gl fallback: OpenGL/GLUT not found in this environment.\n"
            << "Please install OpenGL + FreeGLUT to enable interactive viewer,\n"
            << "or open the generated .vtu file in ParaView.\n";
  return 0;
}
