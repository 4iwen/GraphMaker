#pragma once

#include "vertex.h"

class Edge {
public:
    Edge(Vertex *start, Vertex *end);

    ~Edge();

    Vertex *getStart() const;

    Vertex *getEnd() const;

    Vertex *start;
    Vertex *end;
};