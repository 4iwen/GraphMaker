/*#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <cstdio>

#define SDL_MAIN_HANDLED

#include "SDL.h"
#include "vertex.h"
#include "edge.h"
#include "graph.h"
#include <string>

int main(int, char **) {

    Graph graph;

    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("GraphMaker", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

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

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(1280, 720));
            ImGui::Begin("Window", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoCollapse);

            //static Graph graph;
            //static ImVector<Vertex *> vertices;
            //static ImVector<Edge *> edges;

            static ImVector<ImVec2> edge_points;
            static ImVector<ImVec2> vertex_points;
            static bool adding_edge = false;
            static bool adding_vertex = false;
            const ImVec2 vertex_rect_size(24, 24);
            const ImColor draw_color(255, 255, 0, 255);
            const ImColor outline_color(204, 153, 0, 255);
            const ImColor grid_line_color(200, 200, 200, 40);

            const char *modes[] = {"Add Vertex", "Add Edge", "Remove"};
            static int current_mode = 0;

            const char *algorithms[] = {};
            static int current_algorithm = 0;

            const char *vertices[] = {};
            static int current_vertex = 0;

            // Mode combo
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Mode", &current_mode, modes, IM_ARRAYSIZE(modes));

            // Algorithm combo
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Algorithm", &current_algorithm, algorithms, IM_ARRAYSIZE(algorithms));

            // Start vertex combo
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100);
            ImGui::Combo("Start Vertex", &current_vertex, vertices, IM_ARRAYSIZE(vertices));

            // Clear button
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 220);
            if (ImGui::Button("Clear", ImVec2(100, 0))) {
                graph.vertices.clear();
                edge_points.clear();
            }

            // Run button
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110);
            if (ImGui::Button("Run", ImVec2(100, 0))) {

            }
            ImGui::Spacing();

            // Canvas
            ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
            ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
            if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
            if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
            ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

            // Draw border and background color
            ImGuiIO &io = ImGui::GetIO();
            ImDrawList *draw_list = ImGui::GetWindowDrawList();
            draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
            draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

            // This will catch our interactions
            bool canvas_hovered = false;
            if (current_mode != 2) {
                ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft);
                canvas_hovered = ImGui::IsItemHovered(); // Hovered
            }

            const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvas_p0.x, io.MousePos.y - canvas_p0.y);

            // Handle interactions
            if (current_mode == 0) {
                if (canvas_hovered && !adding_vertex && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    Vertex *vertex = new Vertex(graph.vertices.Size + 1, mouse_pos_in_canvas);
                    graph.addVertex(vertex);
                    adding_vertex = true;
                }
                if (adding_vertex && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                    adding_vertex = false;
                }
            } else if (current_mode == 1) {
                for (int n = 0; n < graph.vertices.Size; n++) {
                    if (canvas_hovered && !adding_edge && graph.vertices[n]->pressed && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        edge_points.push_back(mouse_pos_in_canvas);
                        edge_points.push_back(mouse_pos_in_canvas);
                        adding_edge = true;
                    }
                    if (adding_edge) {
                        edge_points.back() = mouse_pos_in_canvas;
                        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                            adding_edge = false;
                    }
                }

                //if (canvas_hovered && !adding_edge && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                //    edge_points.push_back(mouse_pos_in_canvas);
                //    edge_points.push_back(mouse_pos_in_canvas);
                //    adding_edge = true;
                //}
                //if (adding_edge) {
                //    edge_points.back() = mouse_pos_in_canvas;
                //    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                //        adding_edge = false;
                //}

            }

            // Draw
            draw_list->PushClipRect(canvas_p0, canvas_p1, true);
            // Draw grid
            const float GRID_STEP = 64.0f;
            for (float x = GRID_STEP; x < canvas_sz.x; x += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y),
                                   grid_line_color);
            for (float y = GRID_STEP; y < canvas_sz.y; y += GRID_STEP)
                draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y),
                                   grid_line_color);
            // Draw all edges
            for (int n = 0; n < edge_points.Size; n += 2)
                draw_list->AddLine(ImVec2(canvas_p0.x + edge_points[n].x, canvas_p0.y + edge_points[n].y),
                                   ImVec2(canvas_p0.x + edge_points[n + 1].x, canvas_p0.y + edge_points[n + 1].y),
                                   draw_color, 2.0f);
            // Draw all vertices

            //for (int n = 0; n < vertex_points.Size; n++) {
            //    draw_list->AddRectFilled(ImVec2(canvas_p0.x + vertex_points[n].x - vertex_rect_size.x,
            //                                    canvas_p0.y + vertex_points[n].y - vertex_rect_size.y),
            //                             ImVec2(canvas_p0.x + vertex_points[n].x + vertex_rect_size.x,
            //                                    canvas_p0.y + vertex_points[n].y + vertex_rect_size.y),
            //                             draw_color, 4.0f);
            //    draw_list->AddRect(ImVec2(canvas_p0.x + vertex_points[n].x - vertex_rect_size.x,
            //                              canvas_p0.y + vertex_points[n].y - vertex_rect_size.y),
            //                       ImVec2(canvas_p0.x + vertex_points[n].x + vertex_rect_size.x,
            //                              canvas_p0.y + vertex_points[n].y + vertex_rect_size.y),
            //                       outline_color, 4.0f, 0, 3.0f);
            //}

            for (int n = 0; n < graph.vertices.Size; n++) {
                graph.vertices[n]->draw(draw_list, canvas_p0);
            }
            draw_list->PopClipRect();

            ImGui::End();
        }

        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
*/

#include <iostream>
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer.h"
#include <cstdio>
#include <cmath>

#define SDL_MAIN_HANDLED

#include "SDL.h"

// Vertex structure
struct Vertex {
    ImVec2 pos;
};

// Edge structure
struct Edge {
    int v1, v2; // Indices of the vertices this edge connects
};

// Vector to store the vertices
std::vector<Vertex> vertices;

// Vector to store the edges
std::vector<Edge> edges;

// Function to draw the canvas
void DrawCanvas() {
    ImGuiIO &io = ImGui::GetIO();

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos(); // Top-left corner of the canvas
    ImVec2 canvas_size = ImGui::GetContentRegionAvail(); // Size of the canvas
    if (canvas_size.x < 50.0f) canvas_size.x = 50.0f;
    if (canvas_size.y < 50.0f) canvas_size.y = 50.0f;
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                       IM_COL32(50, 50, 50, 255));

    // Draw edges
    for (int i = 0; i < edges.size(); i++) {
        ImVec2 v1 = vertices[edges[i].v1].pos;
        ImVec2 v2 = vertices[edges[i].v2].pos;
        draw_list->AddLine(ImVec2(canvas_pos.x + v1.x, canvas_pos.y + v1.y),
                           ImVec2(canvas_pos.x + v2.x, canvas_pos.y + v2.y), IM_COL32(255, 255, 255, 255), 2.0f);
    }

    // Draw vertices
    for (int i = 0; i < vertices.size(); i++) {
        ImVec2 pos = ImVec2(canvas_pos.x + vertices[i].pos.x, canvas_pos.y + vertices[i].pos.y);
        draw_list->AddCircleFilled(pos, 8.0f, IM_COL32(255, 255, 255, 255));
    }
}

int main() {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }
    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("GraphMaker", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Main loop
    while (true) {
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Draw the canvas
        DrawCanvas();

        // Handle mouse input
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            mouse_pos.x -= ImGui::GetCursorScreenPos().x;
            mouse_pos.y -= ImGui::GetCursorScreenPos().y;

            // Check if the mouse is inside the canvas
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();
            if (mouse_pos.x >= canvas_pos.x && mouse_pos.x < canvas_pos.x + canvas_size.x &&
                mouse_pos.y >= canvas_pos.y && mouse_pos.y < canvas_pos.y + canvas_size.y) {
                // Add a new vertex at the mouse position
                Vertex v;
                v.pos = mouse_pos;
                vertices.push_back(v);
            }
        }

        int selected_vertex;

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            mouse_pos.x -= ImGui::GetCursorScreenPos().x;
            mouse_pos.y -= ImGui::GetCursorScreenPos().y;

            // Move the selected vertex
            if (selected_vertex != -1) {
                vertices[selected_vertex].pos = mouse_pos;

                // Update edges
                for (int i = 0; i < edges.size(); i++) {
                    if (edges[i].v1 == selected_vertex || edges[i].v2 == selected_vertex) {
                        ImVec2 v1 = vertices[edges[i].v1].pos;
                        ImVec2 v2 = vertices[edges[i].v2].pos;
                    }
                }
            }
        }

        // Handle vertex selection and deletion
        for (int i = 0; i < vertices.size(); i++) {
            ImVec2 pos = ImVec2(ImGui::GetCursorScreenPos().x + vertices[i].pos.x,
                                ImGui::GetCursorScreenPos().y + vertices[i].pos.y);

            ImRect vertex_rect(ImVec2(pos.x - 8, pos.y - 8), ImVec2(pos.x + 8, pos.y + 8));

            bool vertex_hovered = ImGui::IsMouseHoveringRect(vertex_rect.Min, vertex_rect.Max);

            if (vertex_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                selected_vertex = i;
            }

            if (vertex_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                // Remove the vertex and its edges
                vertices.erase(vertices.begin() + i);

                for (int j = edges.size() - 1; j >= 0; j--) {
                    if (edges[j].v1 == i || edges[j].v2 == i) {
                        edges.erase(edges.begin() + j);
                    }
                }

                if (selected_vertex == i) {
                    selected_vertex = -1;
                }
            }
        }

        // Handle edge creation
        if (selected_vertex != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImVec2 mouse_pos = ImGui::GetMousePos();
            mouse_pos.x -= ImGui::GetCursorScreenPos().x;
            mouse_pos.y -= ImGui::GetCursorScreenPos().y;

            // Check if the mouse is inside the canvas
            ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
            ImVec2 canvas_size = ImGui::GetContentRegionAvail();
            if (mouse_pos.x >= canvas_pos.x && mouse_pos.x < canvas_pos.x + canvas_size.x &&
                mouse_pos.y >= canvas_pos.y && mouse_pos.y < canvas_pos.y + canvas_size.y) {
                // Check if the mouse is over another vertex
                for (int i = 0; i < vertices.size(); i++) {
                    // Create a new edge between the selected vertex and the other vertex
                    Edge e;
                    e.v1 = selected_vertex;
                    e.v2 = i;
                    edges.push_back(e);

                    selected_vertex = -1;
                    break;
                }
            }

            // Draw the vertices
            for (int i = 0; i < vertices.size(); i++) {
                ImVec2 pos = ImVec2(ImGui::GetCursorScreenPos().x + vertices[i].pos.x,
                                    ImGui::GetCursorScreenPos().y + vertices[i].pos.y);
                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                draw_list->AddCircleFilled(pos, 8, IM_COL32(255, 255, 255, 255));

                if (selected_vertex == i) {
                    draw_list->AddCircle(pos, 10, IM_COL32(255, 0, 0, 255), 32);
                }
            }

            // Draw the edges
            for (int i = 0; i < edges.size(); i++) {
                ImVec2 v1_pos = ImVec2(ImGui::GetCursorScreenPos().x + vertices[edges[i].v1].pos.y,
                                       ImGui::GetCursorScreenPos().y + vertices[edges[i].v1].pos.y);
                ImVec2 v2_pos = ImVec2(ImGui::GetCursorScreenPos().x + vertices[edges[i].v2].pos.y,
                                       ImGui::GetCursorScreenPos().y + vertices[edges[i].v2].pos.y);
                ImDrawList *draw_list = ImGui::GetWindowDrawList();
                draw_list->AddLine(v1_pos, v2_pos, IM_COL32(255, 255, 255, 255));
            }

            // Render the GUI
            ImGui::End();
            ImGui::Render();
            SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(renderer);
        }

        // Cleanup
        ImGui_ImplSDLRenderer_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

        return 0;
    }
}