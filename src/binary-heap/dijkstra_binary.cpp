#include "dijkstra_binary.hpp"
#include "binary_heap.hpp"

#include <limits>
#include <stdexcept>

std::vector<long long> dijkstraBinary(const Graph& graph, int start, DijkstraStats* stats){
    if (stats != nullptr){
        *stats = DijkstraStats{};
    }

    int vertexCount = static_cast<int>(graph.size());

    if (start < 0 || start >= vertexCount) {
        throw std::out_of_range("Invalid starting vertex");
    }

    const long long INF = std::numeric_limits<long long>::max();

    std::vector<long long> distances(vertexCount, INF);
    std::vector<bool> visited(vertexCount, false);

    BinaryMinHeap heap(vertexCount);

    distances[start] = 0;
    heap.insert(start, 0);
    if (stats){
        stats->insertions++;
    }

    while (!heap.empty()){
        HeapNode current = heap.extractMin();

        if (stats){
            stats->extractions++;
        }

        int u = current.vertex;

        visited[u] = true;

        for (const Edge& edge : graph[u]){
            if (stats){
                stats->edgesExamined++;
            }

            int v = edge.to;

            if (edge.weight < 0){
                throw std::invalid_argument("Negative edge weight");
            }

            if (visited[v]){
                continue;
            }
            long long newDistance = distances[u] + edge.weight;

            if (newDistance < distances[v]){
                if (stats){
                    stats->successfulRelaxations++;
                }

                distances[v] = newDistance;

                if (heap.contains(v)){
                    heap.decreaseKey(v, newDistance);

                    if (stats){
                        stats->decreaseKeys++;
                    }
                }
                else{
                    heap.insert(v, newDistance);

                    if (stats){
                        stats->insertions++;
                    }
                }
            }
        }
    }
    return distances;
}