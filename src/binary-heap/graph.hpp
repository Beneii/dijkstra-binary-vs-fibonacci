#pragma once

#include <vector>
#include <string>

struct Edge{
    int to;
    int weight;
};

using Graph = std::vector<std::vector<Edge>>;

Graph loadGraphCSV(const std::string& filename, int vertexCount);