#include <string>
#include "vertex.h"

Vertex::Vertex(int name, ImVec2 pos) {
    this->value = name;
    this->pos = pos;
    this->visited = false;
    this->pressed = false;
}

Vertex::~Vertex() {

}

void Vertex::draw(ImDrawList *draw_list, ImVec2 canvas_origin) {
    ImGui::SetCursorPos(ImVec2(canvas_origin.x + this->pos.x - 12, canvas_origin.y + this->pos.y - 12));

    ImGui::Button(std::to_string(this->value).c_str(), ImVec2(24, 24));
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            this->pressed = true;
        } else {
            this->pressed = false;
        }
    }
}