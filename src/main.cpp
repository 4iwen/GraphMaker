#include <cstdio>
#include <string>
#include <vector>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"

#define SDL_MAIN_HANDLED

#include "SDL.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

const int VERTEX_SIZE = 12;
const int VERTEX_ROUNDING = 4;
const int EDGE_THICKNESS = 3;

const ImColor VERTEX_COLOR = ImColor(255, 128, 0, 255);
const ImColor EDGE_COLOR = ImColor(255, 255, 0, 255);
const ImColor CANVAS_COLOR = ImColor(50, 50, 50, 255);
const ImColor GRID_COLOR = ImColor(100, 100, 100, 255);
const ImColor SHADOW_COLOR = ImColor(0, 0, 0, 50);

struct Vertex {
    int id;
    ImVec2 pos;
    ImColor color;
};

struct Edge {
    Vertex v1;
    Vertex v2;
    ImColor color;
};

struct Graph {
    ImVector<Vertex> vertices;
    ImVector<Edge> edges;
};

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
        static Vertex v1;

        if (vertex_hovered && !adding_edge && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            v1 = vertex;
            adding_edge = true;

        }

        if (adding_edge && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            bool already_exists = false;

            for (auto &v2: graph.vertices) {
                ImGui::SetCursorScreenPos(ImVec2(v2.pos.x - VERTEX_SIZE, v2.pos.y - VERTEX_SIZE));
                ImGui::InvisibleButton("##v2", ImVec2(VERTEX_SIZE * 2, VERTEX_SIZE * 2),
                                       ImGuiButtonFlags_MouseButtonLeft);
                bool v2_hovered = ImGui::IsItemHovered();

                if (v2_hovered && v2.id != v1.id) {
                    for (auto &edge: graph.edges) {
                        if ((edge.v1.id == v1.id && edge.v2.id == v2.id) ||
                            (edge.v1.id == v2.id && edge.v2.id == v1.id)) {
                            already_exists = true;
                            break;
                        }
                    }
                    if (already_exists) {
                        break;
                    }
                    Edge edge = {v1, v2, EDGE_COLOR};
                    graph.edges.push_back(edge);
                    printf("Edge added: %d -> %d\n", v1.id, v2.id);
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
                if (edge.v1.id == v1->id) {
                    edge.v1.pos = v1->pos;
                }
                if (edge.v2.id == v1->id) {
                    edge.v2.pos = v1->pos;
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

            static const char *algorithms[] = {"BFS", "DFS"};
            static int current_algorithm = 0;
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("Algorithm", &current_algorithm, algorithms, IM_ARRAYSIZE(algorithms));

            static Vertex *start_vertex = nullptr;
            ImGui::SetNextItemWidth(120);
            if (ImGui::BeginCombo("Start", start_vertex ? std::to_string(start_vertex->id).c_str() : "None")) {
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

            if (ImGui::Button("Clear", ImVec2(120, 0))) {
                graph.vertices.clear();
                graph.edges.clear();
            }

            if (ImGui::Button("Run", ImVec2(120, 0))) {

            }

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
                draw_list->AddLine(ImVec2(edge.v1.pos.x, edge.v1.pos.y + 2),
                                   ImVec2(edge.v2.pos.x, edge.v2.pos.y + 2),
                                   SHADOW_COLOR,
                                   EDGE_THICKNESS);
                draw_list->AddLine(edge.v1.pos, edge.v2.pos, edge.color, EDGE_THICKNESS);
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

        ImGui::ShowDemoWindow();

        render(renderer, io);
    }

    cleanup(window, renderer);

    return 0;
}