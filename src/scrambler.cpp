#include "scrambler.h"
#include <cstdlib>
#include <ctime>

// 生成 N 个随机合法的打乱指令序列（避免连续重复同一层）
std::vector<RotationCommand> generateScramble(int count)
{
    std::vector<RotationCommand> sequence;
    Axis lastAxis = AxisX;
    int lastLayer = -1;

    std::srand((unsigned int)std::time(nullptr));

    for (int i = 0; i < count; ++i)
    {
        Axis axis;
        int layer;
        do
        {
            axis = static_cast<Axis>(std::rand() % 3);
            layer = std::rand() % 2;
            if(layer == 1) layer = 2; // 只选0或2层 
        } while (!sequence.empty() && axis == lastAxis && layer == lastLayer);

        bool clockwise = std::rand() % 2 == 0;

        sequence.push_back({axis, layer, clockwise});
        lastAxis = axis;
        lastLayer = layer;
    }
    return sequence;
}