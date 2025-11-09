#pragma once
#include <raylib.h>  // 使用 Raylib 的 Color 结构定义颜色
#include <array>

// 魔方面索引的枚举，方便引用
enum Face { LEFT = 0, RIGHT = 1, DOWN = 2, UP = 3, BACK = 4, FRONT = 5 };

// 坐标轴枚举，用于选择旋转哪一轴方向的层
enum Axis { AxisX = 0, AxisY = 1, AxisZ = 2 };

// 表示一个小立方体（魔方的小块）
struct CubePiece {
    // 每个面一个颜色（如果该面没有贴纸，则设为透明NONE）
    Color faceColor[6];
    
    // 旋转此小方块的朝向颜色（更新贴纸面对应关系）
    void rotateAroundX(bool clockwise);
    void rotateAroundY(bool clockwise);
    void rotateAroundZ(bool clockwise);
};

// 3x3x3 魔方类，包含27个小立方块
class Cube {
public:
    Cube();  // 构造初始化魔方（6面颜色初始化为标准配色）
    
    // 旋转给定轴上某一层（layerIndex=0底/左/背,1中间,2顶/右/前），direction=true顺时针
    void rotateLayer(Axis axis, int layerIndex, bool clockwise);
    
    // 获取小方块指针，供渲染使用
    CubePiece* getPiece(int x, int y, int z) const { return grid[x][y][z]; }

private:
    // 27个小方块的实际存储和3D映射（使用指针方便交换）
    CubePiece pieces[27];
    CubePiece* grid[3][3][3];
    
    // 帮助函数：将某二维平面3x3的九个指针按照顺/逆时针旋转90度
    void rotateFacePointers(Axis axis, int layerIndex, bool clockwise);
};
