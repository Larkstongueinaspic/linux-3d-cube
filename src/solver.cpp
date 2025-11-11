#include "solver.h"
#include "min2phase/min2phase.h"
#include <map>
#include <sstream>
#include <iostream>

// 生成贴纸颜色 → face 字母的映射表（中心块为基准）
static std::map<Color, char> getColorToFaceMap(const Cube &cube)
{
    std::map<Color, char> colorMap;
    // 中心块：6 个面的中间块
    // 默认顺序：U R F D L B
    std::vector<std::tuple<Face, int, int, int, char>> centers = {
        {UP, 1, 2, 1, 'U'},
        {RIGHT, 2, 1, 1, 'R'},
        {FRONT, 1, 1, 2, 'F'},
        {DOWN, 1, 0, 1, 'D'},
        {LEFT, 0, 1, 1, 'L'},
        {BACK, 1, 1, 0, 'B'}};

    for (auto &[face, x, y, z, letter] : centers)
    {
        CubePiece *piece = cube.getPiece(x, y, z);
        Color c = piece->faceColor[face];
        colorMap[c] = letter;
    }
    return colorMap;
}

std::string Solver::encodeFacelets(const Cube &cube)
{
    std::ostringstream result;
    auto colorMap = getColorToFaceMap(cube);

    // facelet 输出顺序：U R F D L B（每面按从左上到右下顺序）
    std::vector<Face> faceOrder = {UP, RIGHT, FRONT, DOWN, LEFT, BACK};

    for (Face face : faceOrder)
    {
        for (int i = 0; i < 3; ++i)
        { // row
            for (int j = 0; j < 3; ++j)
            { // col
                int x, y, z;
                switch (face)
                {
                case UP:
                    x = j;
                    y = 2;
                    z = i;
                    break;
                /**************//**************/
                case DOWN:
                    x = j;
                    y = 0;
                    z = 2 - i;
                /**************//**************/
                    break;
                case LEFT:
                    x = 0;
                    y = 2 - i;
                    z = j;
                    break;
                case RIGHT:
                    x = 2;
                    y = 2 - i;
                    z = 2 - j;
                    break;
                case FRONT:
                    x = j;
                    y = 2 - i;
                    z = 2;
                    break;
                case BACK:
                    x = 2 - j;
                    y = 2 - i;
                    z = 0;
                    break;
                }
                CubePiece *piece = cube.getPiece(x, y, z);
                Color color = piece->faceColor[face];
                char c = colorMap.count(color) ? colorMap[color] : '?';
                result << c;
            }
        }
    }

    return result.str();
}

std::vector<RotationCommandSolver> Solver::solve(const Cube &cube)
{
    min2phase::init();
    std::string facelets = encodeFacelets(cube);
    std::cout << "[Facelets] " << facelets << std::endl;

    std::string sol = min2phase::solve(facelets, 21, 1000000, 0, min2phase::APPEND_LENGTH);
    std::cout << "[Raw Solution] " << sol << std::endl;

    std::vector<RotationCommandSolver> cmds;
    std::istringstream ss(sol);
    std::string move;

    while (ss >> move)
    {
        if (move == "=>")
            break;
        char face = move[0];
        int times = 1;
        bool clockwise = true;

        if (move.length() == 2)
        {
            if (move[1] == '\'')
                clockwise = false;
            else if (move[1] == '2')
                times = 2;
        }

        Axis axis;
        int layer;

        switch (face)
        {
        case 'U':
            axis = AxisY;
            layer = 2;
            clockwise = !clockwise;
            break;
        case 'D':
            axis = AxisY;
            layer = 0;
            break;
        case 'L':
            axis = AxisX;
            layer = 0;
            break;
        case 'R':
            axis = AxisX;
            layer = 2;
            clockwise = !clockwise;
            break;
        case 'F':
            axis = AxisZ;
            layer = 2;
            clockwise = !clockwise;
            break;
        case 'B':
            axis = AxisZ;
            layer = 0;
            break;
        default:
            continue;
        }

        for (int i = 0; i < times; ++i)
        {
            cmds.push_back({axis, layer, clockwise});
        }
    }

    return cmds;
}