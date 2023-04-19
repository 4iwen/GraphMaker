#include <queue>
#include <cstdio>
#include <stack>
#include "graph.h"
#include "SDL_timer.h"

void Graph::bfs(Vertex *start) {
    if (start == nullptr) {
        return;
    }

    std::queue<Vertex *> queue;
    start->visited = true;
    start->color = VISITED_COLOR;
    queue.push(start);

    printf("BFS\n");
    while (!queue.empty()) {
        Vertex *current = queue.front();
        printf("%d\n", current->id);
        queue.pop();

        for (auto &edge : edges) {
            if (edge.from->id == current->id && !edge.to->visited) {
                edge.to->visited = true;
                edge.to->color = VISITED_COLOR;
                queue.push(edge.to);
            }
        }
    }
}

void Graph::dfs(Vertex *start) {
    if (start == nullptr) {
        return;
    }

    std::stack<Vertex *> stack;
    start->visited = true;
    start->color = VISITED_COLOR;
    stack.push(start);

    printf("DFS\n");
    while (!stack.empty()) {
        Vertex *current = stack.top();
        printf("%d\n", current->id);
        stack.pop();

        for (auto &edge : edges) {
            if (edge.from->id == current->id && !edge.to->visited) {
                edge.to->visited = true;
                edge.to->color = VISITED_COLOR;
                stack.push(edge.to);
            }
        }
    }
}

void Graph::reset() {
    for (auto &vertex : vertices) {
        vertex.visited = false;
        vertex.color = VERTEX_COLOR;
        printf("Resetting vertex %d, visited = %d\n", vertex.id, vertex.visited);
    }
}
