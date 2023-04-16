#pragma once

#include "vertex.h"
#include "edge.h"

class Graph {
public:
    Graph();

    ~Graph();

    void addVertex(Vertex *vertex);

    void addEdge(Edge *edge);

    void dump();

    ImVector<Vertex *> vertices;
    ImVector<Edge *> edges;
};
