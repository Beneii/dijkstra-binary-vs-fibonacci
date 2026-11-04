#pragma once

#include <vector>
#include <utility>
#include <stdexcept>

struct HeapNode {
    int vertex;
    long long distance;
};

class BinaryMinHeap {
    private:
        std::vector<HeapNode> heap;
        std::vector<int> position;

        void siftUp(int index) {
            while (index > 0) {
                int parent = (index - 1) / 2;

                if (heap[parent].distance <= heap[index].distance) {
                    break;
                }

                swapNodes(parent, index);
                index = parent;
            }
        }

        void siftDown(int index) {
            int size = static_cast<int>(heap.size());

            while (true) {
                int left = 2 * index + 1;
                int right = 2 * index + 2;
                int smallest = index;

                if (left < size && heap[left].distance < heap[smallest].distance) {
                    smallest = left;
                }
            
                if (right < size && heap[right].distance < heap[smallest].distance) {
                    smallest = right;
                }

                if (smallest == index) {
                    break;
                }

                swapNodes(index, smallest);
                index = smallest;
            }

        }

        void swapNodes(int a, int b) {
            std::swap(heap[a], heap[b]);

            position[heap[a].vertex] = a;
            position[heap[b].vertex] = b;
        }

        
    
    public:
        BinaryMinHeap(int vertexCount)
            : position(vertexCount, -1) {
            }

        void insert(int vertex, long long distance) {
            if (position.at(vertex) != -1) {
                throw std::logic_error("Vertex already in heap");
            }

            int index = static_cast<int>(heap.size());

            heap.push_back({vertex, distance});
            position[vertex] = index;

            siftUp(index);
        }

        const HeapNode& peekMin() const {
            if (heap.empty()) {
                throw std::logic_error("Heap is empty");
            }

            return heap[0];
        }

        HeapNode extractMin() {
            if (heap.empty()) {
                throw std::logic_error("Heap is empty");
            }

            HeapNode minNode = heap[0];

            int lastIndex = static_cast<int>(heap.size()) - 1;

            swapNodes(0, lastIndex);
            heap.pop_back();

            position[minNode.vertex] = -1;

            if (!heap.empty()) {
                siftDown(0);
            }

            return minNode;
        }
        
        void decreaseKey(int vertex, long long newDistance) {
            int index = position.at(vertex);

            if (index == -1) {
                throw std::logic_error("Vertex is not in heap");
            }

            if (newDistance > heap[index].distance) {
                throw std::invalid_argument("New distance cannot be greater than current distance");
            }

            if (newDistance == heap[index].distance) {
                return;
            }

            heap[index].distance = newDistance;

            siftUp(index);
        }

        bool empty() const {
            return heap.empty();
        }

        bool contains(int vertex) const {
            return position.at(vertex) != -1;
        }
};