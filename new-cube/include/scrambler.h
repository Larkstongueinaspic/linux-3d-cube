#pragma once

#include "cube.h"
#include <vector>

struct RotationCommand
{
    Axis axis;
    int layer;
    bool clockwise;
};

// 生成随机打乱序列（避免连续重复）
std::vector<RotationCommand> generateScramble(int count = 20);