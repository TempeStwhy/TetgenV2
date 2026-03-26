# TetgenV2 (C++/CMake)

一个基于 **TetGen** 的 C++ 工程：

- 输入：Nastran 面网格（`GRID + CTRIA3/CQUAD4`）
- 输出：Nastran 四面体网格（`GRID + CTETRA`）
- 可视化：输出 `.vtu`（可直接用 ParaView 打开）；若系统安装了 VTK，还可编译 `tet_view` 直接查看。

## 1. 构建

```bash
cmake -S . -B build
cmake --build build -j
```

> 若未安装 VTK，`nas2tet` 仍可正常编译和运行。

## 2. 使用

```bash
./build/nas2tet <input.nas> <output_tet.nas> [output.vtu] [tetgen_switches]
```

示例：

```bash
./build/nas2tet data/surface.nas data/volume_tet.nas data/volume_tet.vtu pq1.2a0.01
```

默认 `tetgen_switches` 为 `pq1.2aA`。

## 3. 可视化

### 方案 A（推荐）ParaView

直接打开生成的 `*.vtu`。

### 方案 B（可选）本地 VTK Viewer

若 CMake 检测到 VTK，会生成：

```bash
./build/tet_view data/volume_tet.vtu
```

## 4. 输入格式说明

当前支持以下 Nastran 卡片：

- `GRID`
- `CTRIA3`
- `CQUAD4`（自动拆分为两个三角形）

建议输入面网格是封闭、无自交的外表面。
