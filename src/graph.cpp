#include <queue>
#include <cstdio>
#include <stack>
#include "graph.h"

ImVector<Vertex *> Graph::bfs(Vertex *start) {
    if (start == nullptr) {
        return {};
    }

    std::queue<Vertex *> queue;
    start->visited = true;
    queue.push(start);

    ImVector<Vertex *> path;

    printf("BFS\n");
    while (!queue.empty()) {
        Vertex *current = queue.front();
        printf("%d\n", current->id);
        queue.pop();
        path.push_back(current);

        for (auto &edge : edges) {
            if (edge.from->id == current->id && !edge.to->visited) {
                edge.to->visited = true;
                queue.push(edge.to);
            }
        }
    }

    return path;
}

ImVector<Vertex *> Graph::dfs(Vertex *start) {
    if (start == nullptr) {
        return {};
    }

    std::stack<Vertex *> stack;
    start->visited = true;
    stack.push(start);

    ImVector<Vertex *> path;

    printf("DFS\n");
    while (!stack.empty()) {
        Vertex *current = stack.top();
        printf("%d\n", current->id);
        stack.pop();
        path.push_back(current);

        for (auto &edge : edges) {
            if (edge.from->id == current->id && !edge.to->visited) {
                edge.to->visited = true;
                stack.push(edge.to);
            }
        }
    }

    // print the path
    printf("Path:\n");
    for (auto &vertex : path) {
        printf("%d\n", vertex->id);
    }

    return path;
}

void Graph::reset() {
    for (auto &vertex : vertices) {
        vertex.visited = false;
        vertex.color = VERTEX_COLOR;
        printf("Resetting vertex %d, visited = %d\n", vertex.id, vertex.visited);
    }
}
