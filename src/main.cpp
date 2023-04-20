#define IMGUI_DEFINE_MATH_OPERATORS
#include <string>
#include "main.h"
#include "vertex.h"

#include "imgui_internal.h"

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
    ImVector<Vertex *> path;
    int path_index = 0;
    int path_timer = 0;

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
                path.clear();
                path_index = 0;
                path_timer = 0;
            }

            ImGui::SameLine();
            if (ImGui::Button("Run", ImVec2(120, 0))) {
                if (current_algorithm == 0) {
                    path = graph.bfs(start_vertex);
                } else if (current_algorithm == 1) {
                    path = graph.dfs(start_vertex);
                }
            }

            static int speed = 60;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Speed").x - 10);
            ImGui::SliderInt("Speed", &speed, 1, 60, "%d");

            ImGui::Spacing();

            // go through the path each second and highlight the vertices
            if (path_timer == 0) {
                if (path_index < path.Size) {
                    path[path_index]->color = VISITED_COLOR;
                    path_index++;
                }
            }
            path_timer = (path_timer + 1) % speed;

            // draw
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();
            draw(graph, canvas_size);

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