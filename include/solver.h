#pragma once
#include <vector>
#include <string>
#include "cube.h"

// 旋转指令结构（与控制器兼容）
struct RotationCommandSolver {
    Axis axis;
    int layerIndex;
    bool clockwise;
};

class Solver {
public:
    // 从当前魔方状态生成 Min2PhaseCXX 所需的 facelet 字符串
    static std::string encodeFacelets(const Cube& cube);

    // 求解：将当前魔方状态转为字符串并调用 Min2PhaseCXX
    static std::vector<RotationCommandSolver> solve(const Cube& cube);
};