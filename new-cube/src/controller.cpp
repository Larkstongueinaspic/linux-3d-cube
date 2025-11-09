#include "controller.h"
#include <raylib.h>  // 键盘枚举KEY_* 定义
#include <cmath>

// 构造，初始化摄像机和选择状态
Controller::Controller() {
    cameraYaw = 45.0f;    // 初始将视角偏向45度看魔方
    cameraPitch = 30.0f;  // 往上俯视30度
    cameraDistance = 6.0f; // 摄像机距离魔方中心的距离
    selectedAxis = AxisX;
    selectedLayer = 0;
    rotating = false;
    currentAngle = 0.0f;
    rotationSpeed = 6.0f; // 每帧旋转6度，90度约需15帧（~0.25秒）
}

// 每帧调用：处理按键输入并更新状态
void Controller::update(Cube& cube) {
    // 摄像机控制 - WASD 控制视角环绕
    float angleStep = 2.0f;  // 每帧调整角度步长
    if (IsKeyDown(KEY_A)) {   // 左旋转视角（绕Y轴增加偏航角）
        cameraYaw -= angleStep;
    }
    if (IsKeyDown(KEY_D)) {   // 右旋转视角
        cameraYaw += angleStep;
    }
    if (IsKeyDown(KEY_W)) {   // 上视角（增加俯仰角，但有限制）
        cameraPitch += angleStep;
        if (cameraPitch > 85.0f) cameraPitch = 85.0f;   // 防止过顶
    }
    if (IsKeyDown(KEY_S)) {   // 下视角
        cameraPitch -= angleStep;
        if (cameraPitch < -85.0f) cameraPitch = -85.0f; // 防止过底
    }

    // 限制 yaw 在 0-360 (可选)
    if (cameraYaw < 0) cameraYaw += 360.0f;
    if (cameraYaw >= 360.0f) cameraYaw -= 360.0f;
    
    // 如果当前没有旋转动画进行，处理层选择和旋转输入
    if (!rotating) {
        // 方向键选择轴和层：←→ 切换轴，↑↓ 切换层编号
        if (IsKeyPressed(KEY_LEFT)) {
            // 切换到前一个轴 (X->Z->Y->X)
            selectedAxis = static_cast<Axis>((static_cast<int>(selectedAxis) + 2) % 3);
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            // 切换到下一个轴
            selectedAxis = static_cast<Axis>((static_cast<int>(selectedAxis) + 1) % 3);
        }
        if (IsKeyPressed(KEY_UP)) {
            // 层索引 0-2 循环增加
            selectedLayer = (selectedLayer + 1) % 3;
        }
        if (IsKeyPressed(KEY_DOWN)) {
            // 层索引 循环减少
            selectedLayer = (selectedLayer + 2) % 3;  // +2 模3 等效于 -1 模3
        }
        // 当按下 J 或 K 键时，启动旋转动画
        if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_K)) {
            rotating = true;
            rotAxis = selectedAxis;
            rotLayer = selectedLayer;
            // J 设为顺时针，K 逆时针
            rotClockwise = IsKeyPressed(KEY_J);
            currentAngle = 0.0f;
        }
    }
    else {
        // 正在旋转动画中：更新角度
        if (rotClockwise) {
            currentAngle += rotationSpeed;
        } else {
            currentAngle -= rotationSpeed;
        }
        // 完成旋转时（绝对角度达到或超过90度）
        if ((rotClockwise && currentAngle >= 90.0f) || (!rotClockwise && currentAngle <= -90.0f)) {
            // 强制将角度调整为 ±90 完成位置
            currentAngle = rotClockwise ? 90.0f : -90.0f;
            // 调用 Cube 的 rotateLayer 更新魔方数据结构
            cube.rotateLayer(rotAxis, rotLayer, rotClockwise);
            // 重置动画状态
            rotating = false;
            currentAngle = 0.0f;
        }
    }
}
