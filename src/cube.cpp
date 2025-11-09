#include "cube.h"
#include "raymath.h"   // 提供 Vector3Add、Vector3Rotate 等
#include <cmath>

Cube::Cube(float gap_): gap(gap_) { initSolved(); }

void Cube::initSolved(){
    // initialize cubies with solved-color stickers
    // mapping: +X=RED, -X=ORANGE, +Y=WHITE, -Y=YELLOW, +Z=BLUE, -Z=GREEN
    Color fc[6] = { RED, ORANGE, WHITE, YELLOW, BLUE, GREEN };
    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 3; ++y)
            for (int z = 0; z < 3; ++z)
            {
                Cubie &c = cubies[x][y][z];
                c.ix = x;
                c.iy = y;
                c.iz = z;
                for (int f = 0; f < 6; ++f)
                    c.sticker[f] = (Color){0, 0, 0, 255};
                // assign face colors for outer cubies
                if (x == 2)
                    c.sticker[FX] = fc[0];
                if (x == 0)
                    c.sticker[BX] = fc[1];
                if (y == 2)
                    c.sticker[FY] = fc[2];
                if (y == 0)
                    c.sticker[BY] = fc[3];
                if (z == 2)
                    c.sticker[FZ] = fc[4];
                if (z == 0)
                    c.sticker[BZ] = fc[5];
            }
}

std::array<std::array<std::array<Cubie,3>,3>,3>& Cube::raw(){ return cubies; }

void Cube::draw(){
    for(int x=0;x<3;++x) for(int y=0;y<3;++y) for(int z=0;z<3;++z){
        drawCubie(cubies[x][y][z]);
    }
}

void Cube::drawCubie(const Cubie &c){
    // world position: center at origin, spacing 2.0
    float spacing = 1.05f; // cube size 1.0
    Vector3 pos = { (c.ix-1)*spacing, (c.iy-1)*spacing, (c.iz-1)*spacing };
    // draw body
    DrawCubeV(pos, (Vector3){0.9f,0.9f,0.9f}, GRAY);
    DrawCubeWiresV(pos, (Vector3){0.9f,0.9f,0.9f}, BLACK);
    // draw stickers as thin cubes offset from faces
    float stickerThickness = 0.02f;
    float half = 0.9f/2.0f;
    // +X face
    Vector3 off = {half + stickerThickness/2, 0, 0};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.02f,0.7f,0.7f}, c.sticker[FX]);
    // -X
    off = {-half - stickerThickness/2, 0, 0};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.02f,0.7f,0.7f}, c.sticker[BX]);
    // +Y
    off = {0, half + stickerThickness/2, 0};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.7f,0.02f,0.7f}, c.sticker[FY]);
    // -Y
    off = {0, -half - stickerThickness/2, 0};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.7f,0.02f,0.7f}, c.sticker[BY]);
    // +Z
    off = {0,0, half + stickerThickness/2};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.7f,0.7f,0.02f}, c.sticker[FZ]);
    // -Z
    off = {0,0, -half - stickerThickness/2};
    DrawCubeV(Vector3Add(pos, off), (Vector3){0.7f,0.7f,0.02f}, c.sticker[BZ]);
}

static void rotateCubieStickers(std::array<Color,6> &s, int axis, int dir){
    // axis: 0=X,1=Y,2=Z; dir: +1 90deg, -1 -90deg
    // axis: 旋转轴，0为x轴，1为y轴，2为z轴
    // dir：旋转方向，+1：顺时针90度，-1：逆时针90度
    std::array<Color,6> old = s;        //暂存当前各面颜色
    if(axis==1){ // Y axis rotation
        // FX <= FZ, FZ <= BX, BX <= BZ, BZ <= FX  (cw looking from +Y)
        if(dir==1){
            s[FX] = old[FZ]; s[FZ] = old[BX]; s[BX] = old[BZ]; s[BZ] = old[FX];
            s[FY] = old[FY]; s[BY]= old[BY];        //将单个小方块的各个面的颜色进行调换，此时还没进行小方块的移动
        } else {
            s[FX] = old[BZ]; s[FZ] = old[FX]; s[BX] = old[FZ]; s[BZ] = old[BX];
            s[FY] = old[FY]; s[BY]= old[BY];
        }
    } else if(axis==0){ // X axis
        if(dir==1){
            s[FY] = old[BZ]; s[FZ] = old[FY]; s[BY] = old[FZ]; s[BZ] = old[BY];
            s[FX]=old[FX]; s[BX]=old[BX];
        } else {
            s[FY] = old[FZ]; s[FZ] = old[BY]; s[BY] = old[BZ]; s[BZ] = old[FY];
            s[FX]=old[FX]; s[BX]=old[BX];
        }
    } else { // Z axis
        if(dir==1){
            s[FX] = old[BY]; s[BY] = old[BX]; s[BX] = old[FY]; s[FY] = old[FX];
            s[FZ]=old[FZ]; s[BZ]=old[BZ];
        } else {
            s[FX] = old[FY]; s[BY] = old[FX]; s[BX] = old[BY]; s[FY] = old[BX];
            s[FZ]=old[FZ]; s[BZ]=old[BZ];
        }
    }
}

void Cube::permuteLayer(std::array<std::array<std::array<Cubie, 3>, 3>, 3> &tmp, int ax, int layer, int dir)
{   // dir:旋转角度，+1：顺时针90度，-1：逆时针90度
    // ax: 0=X,1=Y,2=Z，判断旋转轴
    // copy current
    tmp = cubies;
    auto idx = [&](int a, int b, int c) -> Cubie &
    { return cubies[a][b][c]; };
    // permute positions and sticker orientations
    if (ax == 1)
    { // rotate around Y-axis: layer is y index
        //绕y轴进行旋转
        for (int x = 0; x < 3; ++x)
            for (int z = 0; z < 3; ++z)
            {
                int nx = (dir == 1) ? z : (2 - z);
                int nz = (dir == 1) ? (2 - x) : x;
                cubies[nx][layer][nz] = tmp[x][layer][z];
                rotateCubieStickers(cubies[nx][layer][nz].sticker, 1, dir);
                cubies[nx][layer][nz].ix = nx;
                cubies[nx][layer][nz].iy = layer;
                cubies[nx][layer][nz].iz = nz;
            }
    }
    else if (ax == 0)
    { // X-axis, layer is x index
        for (int y = 0; y < 3; ++y)
            for (int z = 0; z < 3; ++z)
            {
                int ny = (dir == 1) ? z : (2 - z);
                int nz = (dir == 1) ? (2 - y) : y;
                cubies[layer][ny][nz] = tmp[layer][y][z];
                rotateCubieStickers(cubies[layer][ny][nz].sticker, 0, -dir);
                cubies[layer][ny][nz].ix = layer;
                cubies[layer][ny][nz].iy = ny;
                cubies[layer][ny][nz].iz = nz;
            }
    }
    else
    { // Z-axis, layer is z index
        for (int x = 0; x < 3; ++x)
            for (int y = 0; y < 3; ++y)
            {
                int nx = (dir == 1) ? y : (2 - y);
                int ny = (dir == 1) ? (2 - x) : x;
                cubies[nx][ny][layer] = tmp[x][y][layer];
                rotateCubieStickers(cubies[nx][ny][layer].sticker, 2, -dir);
                cubies[nx][ny][layer].ix = nx;
                cubies[nx][ny][layer].iy = ny;
                cubies[nx][ny][layer].iz = layer;
            }
    }
}

void Cube::rotateLayerY(int layerIndex, int dir){
    std::array<std::array<std::array<Cubie,3>,3>,3> tmp;
    permuteLayer(tmp, 1, layerIndex, dir);
}

void Cube::rotateLayerX(int layerIndex, int dir){
    std::array<std::array<std::array<Cubie,3>,3>,3> tmp;
    permuteLayer(tmp, 0, layerIndex, dir);
}

void Cube::rotateLayerZ(int layerIndex, int dir){
    std::array<std::array<std::array<Cubie,3>,3>,3> tmp;
    permuteLayer(tmp, 2, layerIndex, dir);
}

void Cube::randomize(int moves){
    for(int i=0;i<moves;++i){
        int ax = rand()%3;
        int layer = rand()%3;
        int dir = (rand()%2)?1:-1;
        permuteLayer(*new std::array<std::array<std::array<Cubie,3>,3>,3>(cubies), ax, layer, dir);
    }
}