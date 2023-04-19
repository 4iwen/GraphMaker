#pragma once

#include "imgui.h"

class Vertex {

#define VERTEX_SIZE 12
#define VERTEX_ROUNDING 4
#define VERTEX_COLOR IM_COL32(255, 128, 0, 255)
#define SHADOW_COLOR IM_COL32(0, 0, 0, 50)
#define VISITED_COLOR IM_COL32(0, 180, 0, 255)
#define NON_VISITED_COLOR IM_COL32(180, 0, 0, 255)

public:
    int id;
    ImVec2 pos;
    ImColor color;
    bool visited = false;
};