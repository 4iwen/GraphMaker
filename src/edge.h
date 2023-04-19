#pragma once

#include "imgui.h"
#include "vertex.h"

class Edge {
#define EDGE_THICKNESS 3
#define EDGE_COLOR IM_COL32(22, 22, 22, 255)
#define SHADOW_COLOR IM_COL32(0, 0, 0, 50)

public:
    Vertex *from;
    Vertex *to;
    ImColor color;
};