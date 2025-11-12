#include "controller.h"
#include <raylib.h> // 键盘枚举KEY_* 定义
#include <cmath>
#include <queue>
#include <iostream>
std::queue<RotationCommand> scrambleQueue;
std::queue<RotationCommandSolver> solverQueue;

bool getVisualClockwise(Axis axis, float angle)
{
    // 输入为 controller 提供的 angle（+为逆时针绕轴）
    // 输出为：从轴的正向看，是否顺时针
    switch (axis)
    {
    case AxisX:
        return angle < 0; // 从 +X 看，负角度为顺时针
    case AxisY:
        return angle > 0; // 从 +Y 看，正角度为顺时针
    case AxisZ:
        return angle < 0; // 从 +Z 看，负角度为顺时针
    }
    return true; // fallback
}

// 构造，初始化摄像机和选择状态
Controller::Controller()
{
    cameraYaw = 45.0f;     // 初始将视角偏向45度看魔方
    cameraPitch = 30.0f;   // 往上俯视30度
    cameraDistance = 9.0f; // 摄像机距离魔方中心的距离
    selectedAxis = AxisX;
    selectedLayer = 0;
    rotating = false;
    currentAngle = 0.0f;
    rotationSpeed = 6.0f; // 每帧旋转6度，90度约需15帧（~0.25秒）
    isHighlight = true;
    isSolving = false;
    isScrambling = false;
    isTurning = false;
}

// 每帧调用：处理按键输入并更新状态
void Controller::update(Cube &cube)
{
    /***************/ /**SCRAMBLING**/ /***************/
    // 若打乱队列非空，优先执行打乱
    if (!scrambleQueue.empty())
    {
        if (!rotating)
        {
            RotationCommand cmd = scrambleQueue.front();
            scrambleQueue.pop();
            rotating = true;
            rotAxis = cmd.axis;
            rotLayer = cmd.layer;
            rotClockwise = cmd.clockwise;
            currentAngle = 0.0f;
        }
        else
        {
            if (rotClockwise)
                currentAngle += rotationSpeed;
            else
                currentAngle -= rotationSpeed;
            // 完成旋转时（绝对角度达到或超过90度）
            if ((rotClockwise && currentAngle >= 90.0f) || (!rotClockwise && currentAngle <= -90.0f))
            {
                // 强制将角度调整为 ±90 完成位置
                currentAngle = rotClockwise ? 90.0f : -90.0f;
                // 调用 Cube 的 rotateLayer 更新魔方数据结构
                cube.rotateLayer(rotAxis, rotLayer, getVisualClockwise(rotAxis, currentAngle));
                // 重置动画状态
                rotating = false;
                currentAngle = 0.0f;
            }
        }
        // return;
    }
    else if (isScrambling)
        isScrambling = false;           // 打乱完成
    /***************/ /***************/ /***************/

    /***************/ /****SOLVING****/ /***************/
    if (!solverQueue.empty())
    {
        if (!rotating)
        {
            RotationCommandSolver cmd = solverQueue.front();
            solverQueue.pop();
            rotating = true;
            rotAxis = cmd.axis;
            rotLayer = cmd.layerIndex;
            rotClockwise = cmd.clockwise;
            currentAngle = 0.0f;
        }
        else
        {
            if (rotClockwise)
                currentAngle += rotationSpeed;
            else
                currentAngle -= rotationSpeed;
            // 完成旋转时（绝对角度达到或超过90度）
            if ((rotClockwise && currentAngle >= 90.0f) || (!rotClockwise && currentAngle <= -90.0f))
            {
                // 强制将角度调整为 ±90 完成位置
                currentAngle = rotClockwise ? 90.0f : -90.0f;
                // 调用 Cube 的 rotateLayer 更新魔方数据结构
                cube.rotateLayer(rotAxis, rotLayer, getVisualClockwise(rotAxis, currentAngle));
                // 重置动画状态
                rotating = false;
                currentAngle = 0.0f;
            }
        }
        // return;
    }
    else if (isSolving)
        isSolving = false;
    /***************/ /***************/ /***************/

    // 摄像机控制 - WASD 控制视角环绕
    float angleStep = 3.0f; // 每帧调整角度步长
    if (IsKeyDown(KEY_A))
    { // 左旋转视角（绕Y轴增加偏航角）
        cameraYaw -= angleStep;
    }
    if (IsKeyDown(KEY_D))
    { // 右旋转视角
        cameraYaw += angleStep;
    }
    if (IsKeyDown(KEY_W))
    { // 上视角（增加俯仰角，但有限制）
        cameraPitch += angleStep;
        if (cameraPitch > 85.0f)
            cameraPitch = 85.0f; // 防止过顶
    }
    if (IsKeyDown(KEY_S))
    { // 下视角
        cameraPitch -= angleStep;
        if (cameraPitch < -85.0f)
            cameraPitch = -85.0f; // 防止过底
    }
    if (IsKeyDown(KEY_Q))
    { // 拉近摄像机
        cameraDistance -= 0.1f;
        if (cameraDistance < 6.0f)
            cameraDistance = 6.0f; // 最小距离限制
    }
    if (IsKeyDown(KEY_E))
    { // 拉远摄像机
        cameraDistance += 0.1f;
        if (cameraDistance > 12.0f)
            cameraDistance = 12.0f; // 最大距离限制
    }
    if (IsKeyPressed(KEY_T)){
        isTurning = true;
        rotating = true;
    }
    // 限制 yaw 在 0-360 (可选)
    if (cameraYaw < 0)
        cameraYaw += 360.0f;
    if (cameraYaw >= 360.0f)
        cameraYaw -= 360.0f;

    // 如果当前没有旋转动画进行，处理层选择和旋转输入
    if (!rotating)
    {
        if (IsKeyPressed(KEY_R) && scrambleQueue.empty())
        {
            auto scramble = generateScramble(20);
            for (auto &cmd : scramble)
                scrambleQueue.push(cmd);
            isScrambling = true;
        }
        if (IsKeyPressed(KEY_U) && solverQueue.empty()) {
            std::cout << "U pressed! Calling Solver..." << std::endl;
            std::vector<RotationCommandSolver> solution = Solver::solve(cube);
            std::cout << "Solver returned " << solution.size() << " steps" << std::endl;
            for (const auto& cmd : solution) 
                solverQueue.push(cmd);
            isSolving = true;
        }
        // 方向键选择轴和层：←→ 切换轴，↑↓ 切换层编号
        if (IsKeyPressed(KEY_Z))
        {
            // 切换到X轴
            if(selectedAxis == AxisX){
                if (selectedLayer == 2) selectedLayer = 0;
                else selectedLayer = 2;
            }
            else{
                selectedLayer = 0;
                selectedAxis = AxisX;
            }
        }
        if (IsKeyPressed(KEY_X))
        {
            // 切换到Z轴
            if(selectedAxis == AxisZ){
                if (selectedLayer == 2) selectedLayer = 0;
                else selectedLayer = 2;
            }
            else{
                selectedLayer = 0;
                selectedAxis = AxisZ;
            }
        }
        if (IsKeyPressed(KEY_C))
        {
            // 切换到Y轴
            if(selectedAxis == AxisY){
                if (selectedLayer == 2) selectedLayer = 0;
                else selectedLayer = 2;
            }
            else{
                selectedLayer = 0;
                selectedAxis = AxisY;
            }
        }
        if (IsKeyPressed(KEY_LEFT))
        {
            // 切换到前一个轴 (X->Z->Y->X)
            selectedAxis = static_cast<Axis>((static_cast<int>(selectedAxis) + 2) % 3);
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            // 切换到下一个轴
            selectedAxis = static_cast<Axis>((static_cast<int>(selectedAxis) + 1) % 3);
        }
        if (IsKeyPressed(KEY_UP))
        {
            // 层索引 0-2 循环增加
            if (selectedLayer == 2) selectedLayer = 0;
            else selectedLayer = 2;
        }
        if (IsKeyPressed(KEY_DOWN))
        {
            // 层索引 循环减少
            if (selectedLayer == 2) selectedLayer = 0;
            else selectedLayer = 2;
        }
        // 当按下 J 或 K 键时，启动旋转动画
        if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_K))
        {
            rotating = true;
            rotAxis = selectedAxis;
            rotLayer = selectedLayer;
            // J 设为顺时针，K 逆时针
            rotClockwise = IsKeyPressed(KEY_J);
            currentAngle = 0.0f;
        }
        if (IsKeyPressed(KEY_P))
        {
            isHighlight = !isHighlight;
            // 是否显示选中层高亮
        }
    }
    else
    {
        // 正在旋转动画中：更新角度
        if (rotClockwise)
        {
            currentAngle += rotationSpeed;
        }
        else
        {
            currentAngle -= rotationSpeed;
        }
        // 完成旋转时（绝对角度达到或超过90度）
        if ((rotClockwise && currentAngle >= 90.0f) || (!rotClockwise && currentAngle <= -90.0f))
        {
            // 强制将角度调整为 ±90 完成位置
            currentAngle = rotClockwise ? 90.0f : -90.0f;
            // 调用 Cube 的 rotateLayer 更新魔方数据结构
            if(!isTurning) cube.rotateLayer(rotAxis, rotLayer, getVisualClockwise(rotAxis, currentAngle));
            else {
                for(int i = 0; i <= 1; i++) for(int j=0;j<=2;j++) cube.rotateLayer(AxisX, j, getVisualClockwise(rotAxis, currentAngle));
                SetTargetFPS(80);
                isTurning = false;
            }
            // 重置动画状态
            rotating = false;
            currentAngle = 0.0f;
        }
    }
}
