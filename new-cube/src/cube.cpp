#include "cube.h"
#include <cstring> // 为使用 memset 设置透明色
#include <cmath>

// 定义一个透明颜色常量，用于表示无贴纸的面
static const Color NONE = {0, 0, 0, 0}; // RGBA全为0即透明
static const Color MYORANGE = {217, 152, 52, 255}; // 橙色
static const Color MYRED = {210-10, 45-10, 31-10, 255}; // 红色
static const Color MYYELLOW = {229, 229, 75, 255}; // 黄色
static const Color MYWHITE = {229, 229, 229, 255}; // 白色
static const Color MYGREEN = {49, 113, 29, 255}; // 绿色
static const Color MYBLUE = {0, 0, 220, 255}; // 蓝色

// Cube构造函数：初始化魔方状态（魔方初始为复原状态，每个面的颜色统一）
Cube::Cube()
{
    // 定义六个面的颜色（标准魔方色：白、黄、红、橙、绿、蓝）
    Color colLeft = MYORANGE;
    Color colRight = MYRED;
    Color colDown = MYYELLOW;
    Color colUp = MYWHITE;
    Color colBack = BLUE;
    Color colFront = MYGREEN;

    // Color colLeft = ORANGE;
    // Color colRight = RED;
    // Color colDown = YELLOW;
    // Color colUp = WHITE;
    // Color colBack = BLUE;
    // Color colFront = GREEN;

    // 遍历所有27个位置 (x=0..2, y=0..2, z=0..2)
    int index = 0;
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                CubePiece &piece = pieces[index++];
                // 默认所有面无色，稍后根据位置设置
                for (int f = 0; f < 6; ++f)
                {
                    piece.faceColor[f] = NONE;
                }
                // 如果在最左侧x=0位置，给左面贴纸
                if (x == 0)
                    piece.faceColor[LEFT] = colLeft;
                // 最右侧x=2位置，右面贴纸
                if (x == 2)
                    piece.faceColor[RIGHT] = colRight;
                // 最底层y=0位置，下表面贴纸
                if (y == 0)
                    piece.faceColor[DOWN] = colDown;
                // 最顶层y=2位置，上表面贴纸
                if (y == 2)
                    piece.faceColor[UP] = colUp;
                // 最后排z=0位置，背面贴纸
                if (z == 0)
                    piece.faceColor[BACK] = colBack;
                // 最前排z=2位置，前面贴纸
                if (z == 2)
                    piece.faceColor[FRONT] = colFront;
                // 设置 grid 对应坐标 指向该 piece
                grid[x][y][z] = &piece;
            }
        }
    }
}

// 旋转某一层 (axis: X/Y/Z, layerIndex: 0/1/2, clockwise: 顺时针或逆时针)
void Cube::rotateLayer(Axis axis, int layerIndex, bool clockwise)
{
    // 1. 先交换该层的 grid 指针（小方块位置交换）
    rotateFacePointers(axis, layerIndex, clockwise);
    // 2. 更新该层上每个小方块的朝向贴纸颜色
    // 注意：grid 映射已经更新，但小方块本身仍然携带旧的贴纸布局，需要同步旋转
    if (axis == AxisX)
    {
        // 遍历该X层上所有小块
        for (int y = 0; y < 3; ++y)
        {
            for (int z = 0; z < 3; ++z)
            {
                grid[layerIndex][y][z]->rotateAroundX(clockwise);
            }
        }
    }
    else if (axis == AxisY)
    {
        for (int x = 0; x < 3; ++x)
        {
            for (int z = 0; z < 3; ++z)
            {
                grid[x][layerIndex][z]->rotateAroundY(clockwise);
            }
        }
    }
    else if (axis == AxisZ)
    {
        for (int x = 0; x < 3; ++x)
        {
            for (int y = 0; y < 3; ++y)
            {
                grid[x][y][layerIndex]->rotateAroundZ(clockwise);
            }
        }
    }
}

// 辅助函数：旋转某层的九个 grid 指针（小块位置）
// 将给定 axis层 (3x3) 的指针矩阵顺时针或逆时针旋转90度
void Cube::rotateFacePointers(Axis axis, int layerIndex, bool clockwise)
{
    // 建立临时3x3阵列保存当前层的指针布局
    CubePiece *temp[3][3];
    if (axis == AxisX)
    {
        // 提取当前 X=layerIndex 切片 (固定x，其它y,z变化)
        for (int y = 0; y < 3; ++y)
            for (int z = 0; z < 3; ++z)
                temp[y][z] = grid[layerIndex][y][z];
        // 旋转 YZ 平面 (顺时针 or 逆时针)
        for (int oldY = 0; oldY < 3; ++oldY)
        {
            for (int oldZ = 0; oldZ < 3; ++oldZ)
            {
                int newY, newZ;
                if (clockwise)
                {
                    newY = oldZ;
                    newZ = 2 - oldY;
                }
                else
                {
                    newY = 2 - oldZ;
                    newZ = oldY;
                }
                grid[layerIndex][newY][newZ] = temp[oldY][oldZ];
            }
        }
    }
    else if (axis == AxisY)
    {
        // 提取当前 Y=layerIndex 切片 (固定y，其它x,z变化)
        for (int x = 0; x < 3; ++x)
            for (int z = 0; z < 3; ++z)
                temp[x][z] = grid[x][layerIndex][z];
        // 旋转 XZ 平面
        for (int oldX = 0; oldX < 3; ++oldX)
        {
            for (int oldZ = 0; oldZ < 3; ++oldZ)
            {
                int newX, newZ;
                if (clockwise)
                {
                    newX = oldZ;
                    newZ = 2 - oldX;
                }
                else
                {
                    newX = 2 - oldZ;
                    newZ = oldX;
                }
                grid[newX][layerIndex][newZ] = temp[oldX][oldZ];
            }
        }
    }
    else if (axis == AxisZ)
    {
        // 提取当前 Z=layerIndex 切片 (固定z，其它x,y变化)
        for (int x = 0; x < 3; ++x)
            for (int y = 0; y < 3; ++y)
                temp[x][y] = grid[x][y][layerIndex];
        // 旋转 XY 平面
        for (int oldX = 0; oldX < 3; ++oldX)
        {
            for (int oldY = 0; oldY < 3; ++oldY)
            {
                int newX, newY;
                if (clockwise)
                {
                    newX = oldY;
                    newY = 2 - oldX;
                }
                else
                {
                    newX = 2 - oldY;
                    newY = oldX;
                }
                grid[newX][newY][layerIndex] = temp[oldX][oldY];
            }
        }
    }
}

// CubePiece 绕 X 轴旋转（调整其6个面贴纸的朝向颜色）
void CubePiece::rotateAroundX(bool clockwise)
{
    Color old[6];
    memcpy(old, faceColor, sizeof(old)); // 备份原始颜色
    if (clockwise)
    {
        // 从 X 轴正向（+X）看：顺时针为 Up → Front → Down → Back → Up
        faceColor[UP] = old[FRONT];
        faceColor[FRONT] = old[DOWN];
        faceColor[DOWN] = old[BACK];
        faceColor[BACK] = old[UP];
        faceColor[LEFT] = old[LEFT];
        faceColor[RIGHT] = old[RIGHT];
    }
    else
    {
        // 逆时针：Up → Back → Down → Front → Up
        faceColor[UP] = old[BACK];
        faceColor[BACK] = old[DOWN];
        faceColor[DOWN] = old[FRONT];
        faceColor[FRONT] = old[UP];
        faceColor[LEFT] = old[LEFT];
        faceColor[RIGHT] = old[RIGHT];
    }
}

// 绕 Y 轴旋转
void CubePiece::rotateAroundY(bool clockwise)
{
    Color old[6];
    memcpy(old, faceColor, sizeof(old));
    if (clockwise)
    {
        // 顺时针 (从Y轴正向即上往下看): Front->Right, Right->Back, Back->Left, Left->Front
        faceColor[RIGHT] = old[FRONT];
        faceColor[BACK] = old[RIGHT];
        faceColor[LEFT] = old[BACK];
        faceColor[FRONT] = old[LEFT];
        faceColor[UP] = old[UP];
        faceColor[DOWN] = old[DOWN];
    }
    else
    {
        // 逆时针: Front->Left, Left->Back, Back->Right, Right->Front
        faceColor[LEFT] = old[FRONT];
        faceColor[BACK] = old[LEFT];
        faceColor[RIGHT] = old[BACK];
        faceColor[FRONT] = old[RIGHT];
        faceColor[UP] = old[UP];
        faceColor[DOWN] = old[DOWN];
    }
}

void CubePiece::rotateAroundZ(bool clockwise)
{
    Color old[6];
    memcpy(old, faceColor, sizeof(old));
    if (clockwise)
    {
        // 从 Z 轴正向（+Z）看：顺时针为 Up → Left → Down → Right → Up
        faceColor[UP] = old[LEFT];
        faceColor[LEFT] = old[DOWN];
        faceColor[DOWN] = old[RIGHT];
        faceColor[RIGHT] = old[UP];
        faceColor[FRONT] = old[FRONT];
        faceColor[BACK] = old[BACK];
    }
    else
    {
        // 逆时针：Up → Right → Down → Left → Up
        faceColor[UP] = old[RIGHT];
        faceColor[RIGHT] = old[DOWN];
        faceColor[DOWN] = old[LEFT];
        faceColor[LEFT] = old[UP];
        faceColor[FRONT] = old[FRONT];
        faceColor[BACK] = old[BACK];
    }
}
