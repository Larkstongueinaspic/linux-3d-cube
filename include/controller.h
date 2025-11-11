#pragma once
#include "cube.h"
#include "scrambler.h"
#include "solver.h"

// 控制器类：处理用户输入和动画状态
class Controller {
public:
    Controller();
    // 更新输入和状态，参数为魔方引用（以便触发旋转更新）
    void update(Cube& cube);
    
    // 摄像机相关的只读获取，用于渲染
    float getCameraYaw() const   { return cameraYaw; }
    float getCameraPitch() const { return cameraPitch; }
    float getCameraDistance() const { return cameraDistance; }
    
    // 当前选择的轴和层
    Axis  getSelectedAxis() const  { return selectedAxis; }
    int   getSelectedLayer() const { return selectedLayer; }
    
    // 当前是否正在旋转动画
    bool isRotating() const { return rotating; }
    // 获取当前旋转动画的参数（用于渲染）
    Axis  getRotationAxis() const    { return rotAxis; }
    int   getRotationLayer() const   { return rotLayer; }
    bool  isRotationClockwise() const { return rotClockwise; }
    float getRotationAngle() const   { return currentAngle; }
    bool getIsHighlight() const { return isHighlight; }
    bool getIsScrambling() const { return isScrambling; }
    bool getIsSolving() const { return isSolving; }
    bool getIsTurning() const { return isTurning; }

private:
    // 摄像机绕魔方的角度和距离
    float cameraYaw;
    float cameraPitch;
    float cameraDistance;
    // 选择的旋转轴和层索引
    Axis  selectedAxis;
    int   selectedLayer;
    // 旋转动画状态
    bool  rotating;
    Axis  rotAxis;
    int   rotLayer;
    bool  rotClockwise;
    float currentAngle;    // 当前已旋转的角度 (度数)
    float rotationSpeed;   // 每帧旋转速度 (度)
    bool isHighlight; // 是否显示选中层高亮
    bool isScrambling;
    bool isSolving;
    bool isTurning;
};
