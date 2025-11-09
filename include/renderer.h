#pragma once
#include "raylib.h"
#include "cube.h"

class Renderer {
public:
    Renderer(int w=1024,int h=768);
    ~Renderer();
    void run();
private:
    int width, height;
    Camera camera;
    Cube cube;
    // selection
    int selX=1, selY=1, selZ=1;
    bool animating=false;
    float animProgress=0.0f;
    int animAxis=1, animLayer=1, animDir=1;

    void processInput();
    void update(float dt);
    void draw();
};