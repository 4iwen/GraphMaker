#pragma once

#include "imgui.h"

class Vertex {

public:
    Vertex(int name, ImVec2 pos);

    ~Vertex();

    void draw(ImDrawList *draw_list, ImVec2 canvas_origin);

    ImVec2 pos;
    int value;
    bool visited;
    bool pressed;
};