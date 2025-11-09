#include "renderer.h"
#include "controller.h"
#include <string>

Renderer::Renderer(int w,int h): width(w), height(h), cube(0.02f){
    camera.position = { 4.0f, 4.0f, 6.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

Renderer::~Renderer(){ }

void Renderer::processInput(){
    // camera control WASD QE
    const float camSpeed = 0.05f;
    if(IsKeyDown(KEY_W)) camera.position.z -= camSpeed;
    if(IsKeyDown(KEY_S)) camera.position.z += camSpeed;
    if(IsKeyDown(KEY_A)) camera.position.x -= camSpeed;
    if(IsKeyDown(KEY_D)) camera.position.x += camSpeed;
    if(IsKeyDown(KEY_Q)) camera.position.y -= camSpeed;
    if(IsKeyDown(KEY_E)) camera.position.y += camSpeed;

    // selection with arrow keys (map to X/Z plane)
    if(IsKeyPressed(KEY_RIGHT)) selX = std::min(2, selX+1);
    if(IsKeyPressed(KEY_LEFT)) selX = std::max(0, selX-1);
    if(IsKeyPressed(KEY_UP)) selZ = std::max(0, selZ-1);
    if(IsKeyPressed(KEY_DOWN)) selZ = std::min(2, selZ+1);

    // JK rotate selected layer around Y axis
    if(!animating){
        if(IsKeyPressed(KEY_J)){
            animAxis = 1; animLayer = selY; animDir = -1; animating = true; animProgress=0.0f;
        }
        if(IsKeyPressed(KEY_K)){
            animAxis = 1; animLayer = selY; animDir = 1; animating = true; animProgress=0.0f;
        }
        // for convenience add U/I for X-axis, O/P for Z-axis
        if(IsKeyPressed(KEY_U)){
            animAxis = 0; animLayer = selX; animDir = -1; animating = true; animProgress=0.0f;
        }
        if(IsKeyPressed(KEY_I)){
            animAxis = 0; animLayer = selX; animDir = 1; animating = true; animProgress=0.0f;
        }
        if(IsKeyPressed(KEY_O)){
            animAxis = 2; animLayer = selZ; animDir = -1; animating = true; animProgress=0.0f;
        }
        if(IsKeyPressed(KEY_P)){
            animAxis = 2; animLayer = selZ; animDir = 1; animating = true; animProgress=0.0f;
        }

        // randomize R, auto-solve placeholder T
        if(IsKeyPressed(KEY_R)) cube.randomize(20);
        if(IsKeyPressed(KEY_T)){
            // stub: not implemented yet
        }
    }
}

void Renderer::update(float dt){
    if(animating){
        animProgress += dt*2.0f; // speed
        if(animProgress >= 1.0f){
            // commit logical rotation
            if(animAxis==1) cube.rotateLayerY(animLayer, animDir);
            else if(animAxis==0) cube.rotateLayerX(animLayer, animDir);
            else cube.rotateLayerZ(animLayer, animDir);
            animating = false; animProgress = 0.0f;
        }
    }
}

void Renderer::draw(){
    BeginMode3D(camera);
    // draw cube centered at origin
    cube.draw();
    // highlight selected layer by drawing a transparent bounding box
    Vector3 center = { (selX-1)*1.05f, (selY-1)*1.05f, (selZ-1)*1.05f };
    DrawCubeWires(center, 1.0f, 1.0f, 1.0f, GOLD);
    EndMode3D();

    // HUD
    DrawText("WASD/QE to move camera. Arrows select X/Z. JK rotate Y-layer. U/I X-axis. O/P Z-axis.", 10, 10, 12, RAYWHITE);
}

void Renderer::run(){
    InitWindow(width, height, "Rubik3D - Day1");
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        processInput();
        update(GetFrameTime());
        BeginDrawing();
        ClearBackground(DARKBLUE);
        draw();
        EndDrawing();
    }
    CloseWindow();
}