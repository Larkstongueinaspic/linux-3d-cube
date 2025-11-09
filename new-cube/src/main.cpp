#include "cube.h"
#include "controller.h"
#include "renderer.h"

int main() {
    // 创建魔方对象、控制器和渲染器
    Cube cube;
    Controller controller;
    Renderer renderer(800, 600);  // 窗口初始化，设置尺寸 800x600
    
    // 主循环，直到窗口关闭
    while (!WindowShouldClose()) {
        // 更新输入和动画状态
        controller.update(cube);
        // 绘制当前帧
        renderer.drawFrame(cube, controller);
    }
    return 0;
}
