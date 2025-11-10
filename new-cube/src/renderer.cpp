#include "renderer.h"
#include <rlgl.h>
#include <raymath.h> // 如果需要使用Raylib数学函数（也可使用cmath）

Renderer::Renderer(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight)
{
    // 初始化窗口和3D摄像机
    InitWindow(screenWidth, screenHeight, "3D Rubik's Cube (Day 1)");
    // 设置摄像机参数
    camera.position = {0.0f, 0.0f, 0.0f}; // 将在每帧根据yaw/pitch计算
    camera.target = {0.0f, 0.0f, 0.0f};   // 魔方中心原点
    camera.up = {0.0f, 1.0f, 0.0f};       // 世界上方向 (Y轴)
    camera.fovy = 45.0f;
    SetTargetFPS(60); // 设置帧率
}

Renderer::~Renderer()
{
    CloseWindow();
}

void Renderer::drawFrame(const Cube &cube, const Controller &controller)
{
    // 根据 controller 的摄像机角度计算摄像机位置 (球坐标转换)
    float yaw = controller.getCameraYaw();
    float pitch = controller.getCameraPitch();
    float distance = controller.getCameraDistance();
    distance *= 1.5f; // 放大距离以更好地看到魔方
    // 将角度转成弧度以计算三角函数
    float radYaw = yaw * (PI / 180.0f);
    float radPitch = pitch * (PI / 180.0f);
    // 计算摄像机位置，使其环绕原点
    camera.position.x = distance * sinf(radYaw) * cosf(radPitch);
    camera.position.y = distance * sinf(radPitch);
    camera.position.z = distance * cosf(radYaw) * cosf(radPitch);
    camera.target = {0.0f, 0.0f, 0.0f}; // 始终看向原点

    BeginDrawing();
    ClearBackground(LIGHTGRAY); // 使用浅灰背景，以便白色贴纸清晰可见

    BeginMode3D(camera);

    bool animating = controller.isRotating();
    Axis rotAxis = controller.getRotationAxis();
    int rotLayer = controller.getRotationLayer();
    float angle = controller.getRotationAngle(); // 保持原值（正/负角度）


    // 遍历所有小立方块
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                CubePiece *piece = cube.getPiece(x, y, z);
                // 计算小块的世界坐标位置 (将魔方中心设为(0,0,0)，每块间隔1单位)
                // 我们将魔方3x3范围设为[-1,1]，因此坐标换算：world = (x-1, y-1, z-1)
                float worldX = (float)(x - 1);
                float worldY = (float)(y - 1);
                float worldZ = (float)(z - 1);

                // 判断该小块是否在当前旋转的层内
                bool inRotatingLayer = false;
                if (animating)
                {
                    if ((rotAxis == AxisX && x == rotLayer) ||
                        (rotAxis == AxisY && y == rotLayer) ||
                        (rotAxis == AxisZ && z == rotLayer))
                    {
                        inRotatingLayer = true;
                    }
                }

                if (animating && inRotatingLayer)
                {
                    // 对处于旋转层的小块应用旋转变换
                    rlPushMatrix();
                    // 定义该层旋转轴经过的中心点（轴线穿过魔方中心或边中心）
                    Vector3 pivot = {0.0f, 0.0f, 0.0f};
                    if (rotAxis == AxisX)
                    {
                        pivot = {worldX, 0.0f, 0.0f}; // X层的轴线通过该层中心 (x定值, y=z=0)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 1.0f, 0.0f, 0.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    else if (rotAxis == AxisY)
                    {
                        pivot = {0.0f, worldY, 0.0f}; // Y层轴线 (y定值)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 0.0f, 1.0f, 0.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    else if (rotAxis == AxisZ)
                    {
                        pivot = {0.0f, 0.0f, worldZ}; // Z层轴线 (z定值)
                        rlTranslatef(pivot.x, pivot.y, pivot.z);
                        rlRotatef(angle, 0.0f, 0.0f, 1.0f);
                        rlTranslatef(-pivot.x, -pivot.y, -pivot.z);
                    }
                    // 旋转后再平移小块到其世界位置
                    rlTranslatef(worldX, worldY, worldZ);

                    // 绘制该小块（黑色底立方体 + 6个有色面）
                    DrawCubeV(Vector3{0, 0, 0}, (Vector3){0.9f, 0.9f, 0.9f}, DARKGRAY);
                    DrawCubeWiresV(Vector3{0, 0, 0}, (Vector3){0.9f, 0.9f, 0.9f}, BLACK);
                    // DrawCube(Vector3{0, 0, 0}, 0.9f, 0.9f, 0.9f, BLACK); // 小块主体
                    // 绘制每个有颜色的面贴纸（用薄板表示）
                    float half = 0.45f; // 小块半边长
                    float pad = 0.01f;  // 贴纸板厚度或偏移
                    // 逐面检查，绘制有贴纸的面
                    if (piece->faceColor[LEFT].a != 0)
                    {
                        DrawCube(Vector3{-half - pad, 0, 0}, 0.02f, 0.7f, 0.7f, piece->faceColor[LEFT]);
                    }
                    if (piece->faceColor[RIGHT].a != 0)
                    {
                        DrawCube(Vector3{half + pad, 0, 0}, 0.02f, 0.7f, 0.7f, piece->faceColor[RIGHT]);
                    }
                    if (piece->faceColor[DOWN].a != 0)
                    {
                        DrawCube(Vector3{0, -half - pad, 0}, 0.7f, 0.02f, 0.7f, piece->faceColor[DOWN]);
                    }
                    if (piece->faceColor[UP].a != 0)
                    {
                        DrawCube(Vector3{0, half + pad, 0}, 0.7f, 0.02f, 0.7f, piece->faceColor[UP]);
                    }
                    if (piece->faceColor[BACK].a != 0)
                    {
                        DrawCube(Vector3{0, 0, -half - pad}, 0.7f, 0.7f, 0.02f, piece->faceColor[BACK]);
                    }
                    if (piece->faceColor[FRONT].a != 0)
                    {
                        DrawCube(Vector3{0, 0, half + pad}, 0.7f, 0.7f, 0.02f, piece->faceColor[FRONT]);
                    }

                    rlPopMatrix();
                }
                else
                {
                    // 普通绘制（无动画或不在旋转层的小块）
                    rlPushMatrix();
                    rlTranslatef(worldX, worldY, worldZ);
                    DrawCubeV(Vector3{0, 0, 0}, (Vector3){0.9f, 0.9f, 0.9f}, DARKGRAY);
                    DrawCubeWiresV(Vector3{0, 0, 0}, (Vector3){0.9f, 0.9f, 0.9f}, BLACK);
                    // DrawCube(Vector3{0, 0, 0}, 0.9f, 0.9f, 0.9f, BLACK);
                    float half = 0.45f, pad = 0.01f;
                    if (piece->faceColor[LEFT].a != 0)
                    {
                        DrawCube(Vector3{-half - pad, 0, 0}, 0.02f, 0.7f, 0.7f, piece->faceColor[LEFT]);
                    }
                    if (piece->faceColor[RIGHT].a != 0)
                    {
                        DrawCube(Vector3{half + pad, 0, 0}, 0.02f, 0.7f, 0.7f, piece->faceColor[RIGHT]);
                    }
                    if (piece->faceColor[DOWN].a != 0)
                    {
                        DrawCube(Vector3{0, -half - pad, 0}, 0.7f, 0.02f, 0.7f, piece->faceColor[DOWN]);
                    }
                    if (piece->faceColor[UP].a != 0)
                    {
                        DrawCube(Vector3{0, half + pad, 0}, 0.7f, 0.02f, 0.7f, piece->faceColor[UP]);
                    }
                    if (piece->faceColor[BACK].a != 0)
                    {
                        DrawCube(Vector3{0, 0, -half - pad}, 0.7f, 0.7f, 0.02f, piece->faceColor[BACK]);
                    }
                    if (piece->faceColor[FRONT].a != 0)
                    {
                        DrawCube(Vector3{0, 0, half + pad}, 0.7f, 0.7f, 0.02f, piece->faceColor[FRONT]);
                    }
                    rlPopMatrix();
                }
            }
        }
    }

    /******************************************/ /******************************************/
    // --- 动态绘制选中层的粗线框和透明罩，使其随旋转动画同步 ---
    if (controller.getIfHighlight())
    {
        if (animating)
        {
            // 构造旋转变换
            rlPushMatrix();

            // 找出旋转层中心点
            float layerCoord = (float)(rotLayer - 1);
            Vector3 pivot = {0};
            if (rotAxis == AxisX)
                pivot = {layerCoord, 0.0f, 0.0f};
            else if (rotAxis == AxisY)
                pivot = {0.0f, layerCoord, 0.0f};
            else if (rotAxis == AxisZ)
                pivot = {0.0f, 0.0f, layerCoord};

<<<<<<< HEAD
=======
            // 旋转角度（注意方向）
            float angleDraw = angle;
>>>>>>> d2aa523fdfc54a376dbea00170552c42f7968068

            // 变换：绕该层中心轴旋转
            rlTranslatef(pivot.x, pivot.y, pivot.z);
            if (rotAxis == AxisX)
<<<<<<< HEAD
                rlRotatef(angle, 1.0f, 0.0f, 0.0f);
            if (rotAxis == AxisY)
                rlRotatef(angle, 0.0f, 1.0f, 0.0f);
            if (rotAxis == AxisZ)
                rlRotatef(angle, 0.0f, 0.0f, 1.0f);
=======
                rlRotatef(angleDraw, 1.0f, 0.0f, 0.0f);
            if (rotAxis == AxisY)
                rlRotatef(angleDraw, 0.0f, 1.0f, 0.0f);
            if (rotAxis == AxisZ)
                rlRotatef(angleDraw, 0.0f, 0.0f, 1.0f);
>>>>>>> d2aa523fdfc54a376dbea00170552c42f7968068
            rlTranslatef(-pivot.x, -pivot.y, -pivot.z);

            // 计算层的最小/最大 corner（与前面相同）
            Vector3 minCorner = {-1.5f, -1.5f, -1.5f};
            Vector3 maxCorner = {1.5f, 1.5f, 1.5f};
            if (rotAxis == AxisX)
            {
                float x = layerCoord;
                minCorner.x = x - 0.5f;
                maxCorner.x = x + 0.5f;
            }
            else if (rotAxis == AxisY)
            {
                float y = layerCoord;
                minCorner.y = y - 0.5f;
                maxCorner.y = y + 0.5f;
            }
            else if (rotAxis == AxisZ)
            {
                float z = layerCoord;
                minCorner.z = z - 0.5f;
                maxCorner.z = z + 0.5f;
            }

            BoundingBox layerBox = {minCorner, maxCorner};

            // 半透明罩子
            Color boxColor = ColorAlpha(GOLD, 0.15f);
            Vector3 center = {
                (minCorner.x + maxCorner.x) / 2,
                (minCorner.y + maxCorner.y) / 2,
                (minCorner.z + maxCorner.z) / 2};
            Vector3 size = {
                maxCorner.x - minCorner.x,
                maxCorner.y - minCorner.y,
                maxCorner.z - minCorner.z};
            DrawCube(center, size.x, size.y, size.z, boxColor);

            DrawBoundingBox(layerBox, GOLD);

            rlPopMatrix();
        }
        // --- 添加：绘制选中层的线框高亮区域 ---
        else
        {
            Axis selAxis = controller.getSelectedAxis();
            int selLayer = controller.getSelectedLayer();

            // 计算该层的起点与终点坐标（魔方范围为 [-1,1]）
            Vector3 minCorner = {-1.5f, -1.5f, -1.5f};
            Vector3 maxCorner = {1.5f, 1.5f, 1.5f};

            // 将所选轴方向压缩为该层范围
            if (selAxis == AxisX)
            {
                float x = (float)(selLayer - 1); // -1, 0, 1
                minCorner.x = x - 0.5f;
                maxCorner.x = x + 0.5f;
            }
            else if (selAxis == AxisY)
            {
                float y = (float)(selLayer - 1);
                minCorner.y = y - 0.5f;
                maxCorner.y = y + 0.5f;
            }
            else if (selAxis == AxisZ)
            {
                float z = (float)(selLayer - 1);
                minCorner.z = z - 0.5f;
                maxCorner.z = z + 0.5f;
            }
            Color boxColor = ColorAlpha(GOLD, 0.2f);
            DrawCube({(minCorner.x + maxCorner.x) / 2,
                      (minCorner.y + maxCorner.y) / 2,
                      (minCorner.z + maxCorner.z) / 2},
                     maxCorner.x - minCorner.x,
                     maxCorner.y - minCorner.y,
                     maxCorner.z - minCorner.z,
                     boxColor);
            // 构造包围盒
            BoundingBox layerBox = {minCorner, maxCorner};
            // 绘制边框
            DrawBoundingBox(layerBox, GOLD); // 可以换成别的亮色如 SKYBLUE、RED、PURPLE
            // --- 添加：绘制选中层的线框高亮区域 ---
        }
    }
    /******************************************/ /******************************************/

    EndMode3D();

    // 文字UI：显示当前选择轴和层，以及操作提示
    int textX = 10, textY = 10;
    Axis axis = controller.getSelectedAxis();
    const char *axisName = (axis == AxisX ? "X" : (axis == AxisY ? "Y" : "Z"));
    int layerIndex = controller.getSelectedLayer();
    DrawText(TextFormat("Selected Layer: Axis %s, Index %d \t Press P to display the highlight", axisName, layerIndex), textX, textY, 20, DARKGRAY);
    DrawText("W/A/S/D: Rotate View   Arrow keys: Select Axis/Layer   J/K: Rotate Layer", textX, textY + 30, 20, DARKGRAY);

    EndDrawing();
}