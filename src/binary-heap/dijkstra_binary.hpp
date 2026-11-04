#pragma once

#include "graph.hpp"
#include <vector>

struct DijkstraStats{
    long long insertions = 0;
    long long extractions = 0;
    long long decreaseKeys = 0;
    long long edgesExamined = 0;
    long long successfulRelaxations = 0;
};

std::vector<long long> dijkstraBinary(const Graph& graph, int start, DijkstraStats* stats = nullptr);