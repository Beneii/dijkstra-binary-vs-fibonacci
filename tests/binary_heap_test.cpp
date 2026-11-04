
#include <iostream>
#include "binary_heap.hpp"

int main() {
    
BinaryMinHeap heap(250);

heap.insert(0, 50);
heap.insert(1, 40);
heap.insert(2, 30);
heap.insert(3, 20);
heap.insert(4, 10);

// The heap has been rearranged several times
heap.decreaseKey(0, 5);
heap.decreaseKey(2, 3);

// Extract both decreased vertices
HeapNode first = heap.extractMin();
HeapNode second = heap.extractMin();

std::cout << first.vertex << ": " << first.distance << '\n';
std::cout << second.vertex << ": " << second.distance << '\n';

    return 0;
}
