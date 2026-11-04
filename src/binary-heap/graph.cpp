#include "graph.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

Graph loadGraphCSV(const std::string& filename, int vertexCount){
    if (vertexCount <= 0){
        throw std::invalid_argument("Invalid vertex count");
    }

    std::ifstream file(filename);

    if (!file.is_open()){
        throw std::runtime_error("Could not open CSV file: " + filename);
    }

    Graph graph(vertexCount);

    std::string line;

    std::getline(file, line);

    int lineNumber = 1;

    while (std::getline(file, line)){
        lineNumber++;

        if (line.empty()){
            continue;
        }

        std::stringstream ss(line);

        std::string sourceStr;
        std::string targetStr;
        std::string weightStr;

        if (!std::getline(ss, sourceStr, ',') || !std::getline(ss, targetStr, ',') || !std::getline(ss, weightStr, ',')){
            throw std::runtime_error("Invalid CSV row at line " + std::to_string(lineNumber));
        }

        int source = std::stoi(sourceStr);
        int target = std::stoi(targetStr);
        int weight = std::stoi(weightStr);

        if (source < 0 || source >= vertexCount || target < 0 || target >= vertexCount){
            throw std::out_of_range("Invalid vertex ID");
        }

        if (weight < 0){
            throw std::invalid_argument("Negative edge weight");
        }

        graph[source].push_back({target, weight});
    }

    return graph;
}