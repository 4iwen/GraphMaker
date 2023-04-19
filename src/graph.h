#include "vertex.h"
#include "edge.h"

class Graph {
#define CANVAS_COLOR IM_COL32(50, 50, 50, 255)
#define GRID_COLOR IM_COL32(100, 100, 100, 255)

public:
    ImVector<Vertex> vertices;
    ImVector<Edge> edges;

    void bfs(Vertex *start);

    void dfs(Vertex *start);

    void reset();
};
