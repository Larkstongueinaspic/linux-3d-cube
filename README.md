# linux-3d-cube

## Rubik3D - Day1 Template

This is a minimal Day1 template for a 3D Rubik-style cube using **raylib** and CMake.

Build & run (tested on Linux/macOS):

```bash
mkdir build && cd build
cmake ..
cmake --build . -- -j
./Rubik3D
```

Notes:
- Uses `FetchContent` to grab `raylib` if not available system-wide. On some systems you might prefer to install raylib via package manager and adjust CMake.
- Controls (Day1):
  - WASD/QE: move camera
  - Arrow keys: select layer (X/Z)
  - J/K: rotate selected Y layer
  - U/I: rotate X axis layer
  - O/P: rotate Z axis layer
  - R: randomize
  - T: (solver stub)