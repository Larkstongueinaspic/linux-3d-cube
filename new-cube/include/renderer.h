#pragma once
#include "cube.h"
#include "controller.h"
#include "scrambler.h"
#include <raylib.h>

class Renderer {
public:
    Renderer(int screenWidth, int screenHeight);
    ~Renderer();
    // 绘制一帧场景
    void drawFrame(const Cube &cube, const Controller &controller);

private:
    Camera3D camera;  // Raylib 3D 摄像机
    int screenWidth;
    int screenHeight;
};
