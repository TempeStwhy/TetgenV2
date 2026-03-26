# TetgenV2 (C++/CMake)

一个基于 **TetGen** 的 C++ 工程：

- 输入：Nastran 面网格（`GRID + CTRIA3/CQUAD4`）
- 输出：Nastran 四面体网格（`GRID + CTETRA`）
- 可视化：
  - 默认：导出 `.vtu`（可直接用 ParaView）
  - 无需 VTK：可选编译 `tet_view_gl`（OpenGL/GLUT）

## 1. 构建

```bash
cmake -S . -B build
cmake --build build -j
```

> - 若 OpenGL/GLUT 可用，会构建 `tet_view_gl`。
> - 若你需要 VTK viewer，可开启：`-DENABLE_VTK_VIEWER=ON`。

## 2. 使用

```bash
./build/nas2tet <input.nas> <output_tet.nas> [output.vtu] [tetgen_switches]
```

示例（仓库已提供合法闭合面网格）：

```bash
./build/nas2tet data/tetra_surface.nas data/volume_tet.nas data/volume_tet.vtu pq1.2a0.01
```

默认 `tetgen_switches` 为 `pq1.2aA`。

## 3. 可视化

### 方案 A：ParaView（推荐）

直接打开生成的 `*.vtu`。

### 方案 B：OpenGL Viewer（无需 VTK）

```bash
./build/tet_view_gl data/volume_tet.nas
```

操作：方向键旋转，`+/-` 缩放，`q`/`ESC` 退出。

### 方案 C：VTK Viewer（可选）

```bash
cmake -S . -B build -DENABLE_VTK_VIEWER=ON
cmake --build build -j
./build/tet_view data/volume_tet.vtu
```

## 4. 输入格式说明

当前支持以下 Nastran 卡片：

- `GRID`
- `CTRIA3`
- `CQUAD4`（自动拆分为两个三角形）

请确保输入是**闭合、二维流形、无自交**的外表面网格（仓库示例 `data/tetra_surface.nas` 为合法案例）。

## 5. 可视化源码位置

已一并提交可视化相关源码：

- `src/opengl_viewer.cpp`：OpenGL/GLUT 线框查看器（`tet_view_gl`）
- `src/vtu_viewer.cpp`：VTK 查看器（`tet_view`）

你可以按需在 CMake 配置时开启/关闭对应目标：

- OpenGL viewer：`-DENABLE_OPENGL_VIEWER=ON|OFF`
- VTK viewer：`-DENABLE_VTK_VIEWER=ON|OFF`

