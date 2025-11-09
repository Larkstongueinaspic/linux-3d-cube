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
    std::array<Color, 6> sticker;// 6个面的贴纸颜色
    // logical coordinates in [-1,0,1] index space
    int ix, iy, iz;             // 在 [-1, 0, 1] 索引空间中的逻辑坐标(0,0,0) = 中心块（实际不存在）(1,0,0) = 右侧面的中心块 (1,1,0) = 右上边缘块
                                // XXXXX
                                // 实际上是[0,1,2]索引空间
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