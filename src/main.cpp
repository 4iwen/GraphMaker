#define IMGUI_DEFINE_MATH_OPERATORS

#include <cstdio>
#include <string>
#include "main.h"
#include "vertex.h"
#include "edge.h"
#include "graph.h"

#include "imgui_internal.h"

void add_vertex(Graph &graph, ImVec2 canvas_size) {
    ImGui::InvisibleButton("canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft);
    bool canvas_hovered = ImGui::IsItemHovered();
    static bool adding_vertex = false;

    if (canvas_hovered && !adding_vertex && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        Vertex vertex = {graph.vertices.Size + 1, ImGui::GetMousePos(), VERTEX_COLOR};
        graph.vertices.push_back(vertex);
        printf("Vertex added: %d\n", vertex.id);
        adding_vertex = true;
    }
    if (adding_vertex && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        adding_vertex = false;
    }
}

void add_edge(Graph &graph) {
    for (auto &vertex: graph.vertices) {
        ImGui::SetCursorScreenPos(ImVec2(vertex.pos.x - VERTEX_SIZE, vertex.pos.y - VERTEX_SIZE));
        ImGui::InvisibleButton("##vertex", ImVec2(VERTEX_SIZE * 2, VERTEX_SIZE * 2), ImGuiButtonFlags_MouseButtonLeft);
        bool vertex_hovered = ImGui::IsItemHovered();
        static bool adding_edge = false;
        static Vertex *from;

        if (vertex_hovered && !adding_edge && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            from = &vertex;
            adding_edge = true;
        }
        if (adding_edge && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            bool already_exists = false;

            for (auto &to: graph.vertices) {
                ImGui::SetCursorScreenPos(ImVec2(to.pos.x - VERTEX_SIZE, to.pos.y - VERTEX_SIZE));
                ImGui::InvisibleButton("##to", ImVec2(VERTEX_SIZE * 2, VERTEX_SIZE * 2),
                                       ImGuiButtonFlags_MouseButtonLeft);
                bool v2_hovered = ImGui::IsItemHovered();

                if (v2_hovered && to.id != from->id) {
                    for (auto &edge: graph.edges) {
                        if ((edge.from->id == from->id && edge.to->id == to.id) ||
                            (edge.from->id == to.id && edge.to->id == from->id)) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (already_exists) {
                        break;
                    }

                    Edge edge = {from, &to, EDGE_COLOR};
                    graph.edges.push_back(edge);
                    printf("Edge added: %d -> %d\n", from->id, to.id);
                    break;
                }
            }
            adding_edge = false;
        }
    }
}

void move(Graph &graph) {
    for (auto &vertex: graph.vertices) {
        ImGui::SetCursorScreenPos(ImVec2(vertex.pos.x - VERTEX_SIZE, vertex.pos.y - VERTEX_SIZE));
        ImGui::InvisibleButton("##vertex", ImVec2(VERTEX_SIZE * 2, VERTEX_SIZE * 2), ImGuiButtonFlags_MouseButtonLeft);
        bool vertex_hovered = ImGui::IsItemHovered();
        static bool moving_vertex = false;
        static Vertex *v1;

        if (vertex_hovered && !moving_vertex && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            v1 = &vertex;
            moving_vertex = true;
        }
        if (moving_vertex && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            v1->pos = ImGui::GetMousePos();
            // update edges
            for (auto &edge: graph.edges) {
                if (edge.from->id == v1->id) {
                    edge.from->pos = v1->pos;
                }
                if (edge.to->id == v1->id) {
                    edge.to->pos = v1->pos;
                }
            }
        }
        if (moving_vertex && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            moving_vertex = false;
        }
    }
}

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;

    if (!init(window, renderer)) {
        return EXIT_FAILURE;
    }

    imgui_setup(window, renderer);

    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    Graph graph;

    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));
        ImGui::Begin("GraphMaker", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse);
        {
            static const char *modes[] = {"Add Vertex", "Add Edge", "Move"};
            static int current_mode = 0;
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

            ImGui::SameLine();
            if (ImGui::Button("Clear Canvas", ImVec2(120, 0))) {
                graph.vertices.clear();
                graph.edges.clear();
            }

            static const char *algorithms[] = {"BFS", "DFS"};
            static int current_algorithm = 0;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("Algorithm", &current_algorithm, algorithms, IM_ARRAYSIZE(algorithms));

            static Vertex *start_vertex = nullptr;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            if (ImGui::BeginCombo("Start Vertex", start_vertex ? std::to_string(start_vertex->id).c_str() : "None")) {
                for (auto &vertex: graph.vertices) {
                    if (ImGui::Selectable(std::to_string(vertex.id).c_str())) {
                        start_vertex = &vertex;
                    }
                }
                ImGui::EndCombo();
            }
            if (graph.vertices.empty()) {
                start_vertex = nullptr;
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset", ImVec2(120, 0))) {
                graph.reset();
            }

            ImGui::SameLine();
            if (ImGui::Button("Run", ImVec2(120, 0))) {
                if (current_algorithm == 0) {
                    graph.bfs(start_vertex);
                } else if (current_algorithm == 1) {
                    graph.dfs(start_vertex);
                }

                // color non-visited vertices red
                for (auto &vertex: graph.vertices) {
                    if (!vertex.visited) {
                        vertex.color = NON_VISITED_COLOR;
                    }
                }
            }
            ImGui::Spacing();

            // draw canvas
            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            ImVec2 canvas_start = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();
            ImVec2 canvas_end = ImVec2(canvas_start.x + canvas_size.x, canvas_start.y + canvas_size.y);
            draw_list->AddRectFilled(canvas_start, canvas_end, CANVAS_COLOR);
            draw_list->AddRect(canvas_start, canvas_end, GRID_COLOR);

            draw_list->PushClipRect(canvas_start, canvas_end, true);
            // draw grid
            for (int i = 0; i < canvas_size.x; i += 64) {
                draw_list->AddLine(ImVec2(canvas_start.x + i, canvas_start.y),
                                   ImVec2(canvas_start.x + i, canvas_end.y),
                                   GRID_COLOR);
            }
            for (int i = 0; i < canvas_size.y; i += 64) {
                draw_list->AddLine(ImVec2(canvas_start.x, canvas_start.y + i),
                                   ImVec2(canvas_end.x, canvas_start.y + i),
                                   GRID_COLOR);
            }

            // draw edges
            for (auto &edge: graph.edges) {
                // line
                draw_list->AddLine(ImVec2(edge.from->pos.x, edge.from->pos.y + 2),
                                   ImVec2(edge.to->pos.x, edge.to->pos.y + 2),
                                   SHADOW_COLOR,
                                   EDGE_THICKNESS);
                draw_list->AddLine(edge.from->pos, edge.to->pos, edge.color, EDGE_THICKNESS);
                // arrow
                ImVec2 edge_dir = edge.to->pos - edge.from->pos;
                ImVec2 edge_dir_norm = edge_dir / sqrt(edge_dir.x * edge_dir.x + edge_dir.y * edge_dir.y);
                ImVec2 edge_dir_perp = ImVec2(-edge_dir_norm.y, edge_dir_norm.x);
                ImVec2 arrow_pos = edge.from->pos + edge_dir * 0.5f;
                draw_list->AddTriangleFilled(
                        arrow_pos + edge_dir_perp * 10.0f + ImVec2(0, 2),
                        arrow_pos - edge_dir_perp * 10.0f + ImVec2(0, 2),
                        arrow_pos + edge_dir_norm * 20.0f + ImVec2(0, 2),
                        SHADOW_COLOR);
                draw_list->AddTriangleFilled(
                        arrow_pos + edge_dir_perp * 10.0f,
                        arrow_pos - edge_dir_perp * 10.0f,
                        arrow_pos + edge_dir_norm * 20.0f,
                        edge.color);
            }

            // draw vertices
            for (int i = 0; i < graph.vertices.Size; i++) {
                // shadow
                draw_list->AddRectFilled(
                        ImVec2(graph.vertices[i].pos.x - VERTEX_SIZE + 2, graph.vertices[i].pos.y - VERTEX_SIZE + 2),
                        ImVec2(graph.vertices[i].pos.x + VERTEX_SIZE + 2, graph.vertices[i].pos.y + VERTEX_SIZE + 2),
                        SHADOW_COLOR,
                        VERTEX_ROUNDING);
                // vertex
                draw_list->AddRectFilled(
                        ImVec2(graph.vertices[i].pos.x - VERTEX_SIZE, graph.vertices[i].pos.y - VERTEX_SIZE),
                        ImVec2(graph.vertices[i].pos.x + VERTEX_SIZE, graph.vertices[i].pos.y + VERTEX_SIZE),
                        graph.vertices[i].color,
                        VERTEX_ROUNDING);
                // text
                const char *text = std::to_string(graph.vertices[i].id).c_str();
                draw_list->AddText(
                        ImVec2(graph.vertices[i].pos.x - ImGui::CalcTextSize(text).x / 2,
                               graph.vertices[i].pos.y - ImGui::CalcTextSize(text).y / 2),
                        IM_COL32_WHITE,
                        text);
            }
            draw_list->PopClipRect();

            switch (current_mode) {
                case 0:
                    add_vertex(graph, canvas_size);
                    break;
                case 1:
                    add_edge(graph);
                    break;
                case 2:
                    move(graph);
                    break;
                default:
                    break;
            }
        }
        ImGui::End();

        render(renderer, io);
    }

    cleanup(window, renderer);

    return 0;
}