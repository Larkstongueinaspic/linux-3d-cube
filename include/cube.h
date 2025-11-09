#pragma once
#include "raylib.h"
#include <array>
#include <vector>

// Faces: +X, -X, +Y, -Y, +Z, -Z
enum Face
{
    FX = 0,
    BX = 1,
    FY = 2,
    BY = 3,
    FZ = 4,
    BZ = 5
};

struct Cubie
{
    // sticker colors by face index
    std::array<Color, 6> sticker;
    // logical coordinates in [-1,0,1] index space
    int ix, iy, iz;
};

class Cube
{
public:
    Cube(float gap = 0.05f);
    void draw();
    void rotateLayerY(int layerIndex, int dir); // dir = +1 (CW) or -1 (CCW) looking from +Y
    void rotateLayerX(int layerIndex, int dir);
    void rotateLayerZ(int layerIndex, int dir);
    void randomize(int moves = 20);
    std::array<std::array<std::array<Cubie, 3>, 3>, 3> & raw();

private:
    std::array<std::array<std::array<Cubie, 3>, 3>, 3> cubies;
    float gap;
    void initSolved();
    void drawCubie(const Cubie &c);
    void permuteLayer(std::array<std::array<std::array<Cubie, 3>, 3>, 3> &tmp,
                      int ax, int layer, int dir);
};