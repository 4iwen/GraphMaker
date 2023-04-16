#include <cstdio>
#include "graph.h"

Graph::Graph() {

}

Graph::~Graph() {

}

void Graph::addVertex(Vertex *vertex) {
    this->vertices.push_back(vertex);
}

void Graph::addEdge(Edge *edge) {
    this->edges.push_back(edge);
}

void Graph::dump() {
    // print the vertices and edges
    for (auto vertex : this->vertices) {
        printf("Vertex: %s\n", vertex->value);
    }
    for (auto edge : this->edges) {
        printf("Edge: %s -> %s\n", edge->start->value, edge->end->value);
    }
}