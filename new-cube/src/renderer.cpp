#include "renderer.h"
#include <rlgl.h>
#include <raymath.h>   // 如果需要使用Raylib数学函数（也可使用cmath）

Renderer::Renderer(int screenWidth, int screenHeight) 
    : screenWidth(screenWidth), screenHeight(screenHeight) {
    // 初始化窗口和3D摄像机
    InitWindow(screenWidth, screenHeight, "3D Rubik's Cube (Day 1)");
    // 设置摄像机参数
    camera.position = { 0.0f, 0.0f, 0.0f };  // 将在每帧根据yaw/pitch计算
    camera.target   = { 0.0f, 0.0f, 0.0f };  // 魔方中心原点
    camera.up       = { 0.0f, 1.0f, 0.0f };  // 世界上方向 (Y轴)
    camera.fovy     = 45.0f;
    SetTargetFPS(60);  // 设置帧率
}

Renderer::~Renderer() {
    CloseWindow();
}

// 利用Controller状态绘制当前帧
void Renderer::drawFrame(const Cube& cube, const Controller& controller) {
    // 根据 controller 的摄像机角度计算摄像机位置 (球坐标转换)
    float yaw = controller.getCameraYaw();
    float pitch = controller.getCameraPitch();
    float distance = controller.getCameraDistance();
    // 将角度转成弧度以计算三角函数
    float radYaw = yaw * (PI / 180.0f);
    float radPitch = pitch * (PI / 180.0f);
    // 计算摄像机位置，使其环绕原点
    camera.position.x = distance * sinf(radYaw) * cosf(radPitch);
    camera.position.y = distance * sinf(radPitch);
    camera.position.z = distance * cosf(radYaw) * cosf(radPitch);
    camera.target = { 0.0f, 0.0f, 0.0f };  // 始终看向原点
    
    BeginDrawing();
    ClearBackground(LIGHTGRAY);  // 使用浅灰背景，以便白色贴纸清晰可见
    
    BeginMode3D(camera);
    
    // 绘制魔方的所有小方块
    bool animating = controller.isRotating();
    Axis rotAxis = controller.getRotationAxis();
    int rotLayer = controller.getRotationLayer();
    float angle = controller.getRotationAngle();
    bool cw = controller.isRotationClockwise();
    
    // 若正在动画且逆时针，则取反角度方便应用
    if (animating && !cw) {
        angle = -angle;
    }
    
    // 遍历所有小立方块
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < 3; ++z) {
                CubePiece* piece = cube.getPiece(x, y, z);
                // 计算小块的世界坐标位置 (将魔方中心设为(0,0,0)，每块间隔1单位)
                // 我们将魔方3x3范围设为[-1,1]，因此坐标换算：world = (x-1, y-1, z-1)
                float worldX = (float)(x - 1);
                float worldY = (float)(y - 1);
                float worldZ = (float)(z - 1);
                
                // 判断该小块是否在当前旋转的层内
                bool inRotatingLayer = false;
                if (animating) {
                    if ((rotAxis == AxisX && x == rotLayer) ||
                        (rotAxis == AxisY && y == rotLayer) ||
                        (rotAxis == AxisZ && z == rotLayer)) {
                        inRotatingLayer = true;
                    }
                }
                
                if (animating && inRotatingLayer) {
                    // 对处于旋转层的小块应用旋转变换
                    rlPushMatrix();
                    // 定义该层旋转轴经过的中心点（轴线穿过魔方中心或边中心）
                    Vector3 pivot = { 0.0f, 0.0f, 0.0f };
                    if (rotAxis == AxisX) {
                        pivot = { worldX, 0.0f, 0.0f };  // X层的轴线通过该层中心 (x定值, y=z=0)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 1.0f, 0.0f, 0.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    else if (rotAxis == AxisY) {
                        pivot = { 0.0f, worldY, 0.0f };  // Y层轴线 (y定值)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 0.0f, 1.0f, 0.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    else if (rotAxis == AxisZ) {
                        pivot = { 0.0f, 0.0f, worldZ };  // Z层轴线 (z定值)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 0.0f, 0.0f, 1.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    // 旋转后再平移小块到其世界位置
                    rlTranslatef(worldX, worldY, worldZ);
                    
                    // 绘制该小块（黑色底立方体 + 6个有色面）
                    DrawCube(Vector3{0,0,0}, 0.9f, 0.9f, 0.9f, BLACK); // 小块主体
                    // 绘制每个有颜色的面贴纸（用薄板表示）
                    float half = 0.45f;   // 小块半边长
                    float pad = 0.01f;    // 贴纸板厚度或偏移
                    // 逐面检查，绘制有贴纸的面
                    if (piece->faceColor[LEFT].a != 0) {
                        DrawCube(Vector3{ -half - pad, 0, 0 }, 0.02f, 0.9f, 0.9f, piece->faceColor[LEFT]);
                    }
                    if (piece->faceColor[RIGHT].a != 0) {
                        DrawCube(Vector3{ half + pad, 0, 0 }, 0.02f, 0.9f, 0.9f, piece->faceColor[RIGHT]);
                    }
                    if (piece->faceColor[DOWN].a != 0) {
                        DrawCube(Vector3{ 0, -half - pad, 0 }, 0.9f, 0.02f, 0.9f, piece->faceColor[DOWN]);
                    }
                    if (piece->faceColor[UP].a != 0) {
                        DrawCube(Vector3{ 0, half + pad, 0 }, 0.9f, 0.02f, 0.9f, piece->faceColor[UP]);
                    }
                    if (piece->faceColor[BACK].a != 0) {
                        DrawCube(Vector3{ 0, 0, -half - pad }, 0.9f, 0.9f, 0.02f, piece->faceColor[BACK]);
                    }
                    if (piece->faceColor[FRONT].a != 0) {
                        DrawCube(Vector3{ 0, 0, half + pad }, 0.9f, 0.9f, 0.02f, piece->faceColor[FRONT]);
                    }
                    
                    rlPopMatrix();
                }
                else {
                    // 普通绘制（无动画或不在旋转层的小块）
                    rlPushMatrix();
                    rlTranslatef(worldX, worldY, worldZ);
                    DrawCube(Vector3{0,0,0}, 0.9f, 0.9f, 0.9f, BLACK);
                    float half = 0.45f, pad = 0.01f;
                    if (piece->faceColor[LEFT].a != 0) {
                        DrawCube(Vector3{ -half - pad, 0, 0 }, 0.02f, 0.9f, 0.9f, piece->faceColor[LEFT]);
                    }
                    if (piece->faceColor[RIGHT].a != 0) {
                        DrawCube(Vector3{ half + pad, 0, 0 }, 0.02f, 0.9f, 0.9f, piece->faceColor[RIGHT]);
                    }
                    if (piece->faceColor[DOWN].a != 0) {
                        DrawCube(Vector3{ 0, -half - pad, 0 }, 0.9f, 0.02f, 0.9f, piece->faceColor[DOWN]);
                    }
                    if (piece->faceColor[UP].a != 0) {
                        DrawCube(Vector3{ 0, half + pad, 0 }, 0.9f, 0.02f, 0.9f, piece->faceColor[UP]);
                    }
                    if (piece->faceColor[BACK].a != 0) {
                        DrawCube(Vector3{ 0, 0, -half - pad }, 0.9f, 0.9f, 0.02f, piece->faceColor[BACK]);
                    }
                    if (piece->faceColor[FRONT].a != 0) {
                        DrawCube(Vector3{ 0, 0, half + pad }, 0.9f, 0.9f, 0.02f, piece->faceColor[FRONT]);
                    }
                    rlPopMatrix();
                }
            }
        }
    }
    
    EndMode3D();
    
    // 文字UI：显示当前选择轴和层，以及操作提示
    int textX = 10, textY = 10;
    Axis axis = controller.getSelectedAxis();
    const char* axisName = (axis == AxisX ? "X" : (axis == AxisY ? "Y" : "Z"));
    int layerIndex = controller.getSelectedLayer();
    DrawText(TextFormat("Selected Layer: Axis %s, Index %d", axisName, layerIndex), textX, textY, 20, DARKGRAY);
    DrawText("W/A/S/D: Rotate View   Arrow keys: Select Axis/Layer   J/K: Rotate Layer", textX, textY + 30, 20, DARKGRAY);
    
    EndDrawing();
}
