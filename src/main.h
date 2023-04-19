#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

#define SDL_MAIN_HANDLED

#include "SDL.h"
#include "graph.h"
#include <cmath>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

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

bool init(SDL_Window *&window, SDL_Renderer *&renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        return false;
    }
    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("GraphMaker", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        return false;
    }
    return true;
}

void imgui_setup(SDL_Window *&window, SDL_Renderer *&renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
}

void draw(Graph &graph, ImVec2 canvas_size) {
    // draw canvas
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_start = ImGui::GetCursorScreenPos();
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
}

void render(SDL_Renderer *&renderer, ImGuiIO &io) {
    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

void cleanup(SDL_Window *&window, SDL_Renderer *&renderer) {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}