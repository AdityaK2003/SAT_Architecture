#include <iostream>
#include <cmath>
#include <iomanip>

#include "heap.h"


// Helper function to "pretty print" the heap
static void heap_pretty_print(Heap h) {
    int n = h.size();
    int height = std::ceil(std::log2(n + 1));

    for (int level = 0; level < height; ++level) {
        int level_start = std::pow(2, level) - 1;
        int level_end = std::min(n - 1, (int)std::pow(2, level + 1) - 2);
        int padding = std::pow(2, height - level - 1) - 1;

        std::cout << std::setw(padding) << "";

        for (int i = level_start; i <= level_end; ++i) {
            std::cout << h.heap[i] << std::setw(2 * padding + 1) << "";
        }
        std::cout << std::endl;
    }
}


int main() {
    // Variable IDs go from 0 to n-1, where n is the number of variables

    std::cout << "\nExample heap usage:\n\n";

    std::cout << "Initialize with 8 variables (x0, x1, ..., x7).\n\n";
    Heap heap(8);
    heap.print_data();

    std::cout << "Bumping activity of variable 7.\n\n";
    heap.var_bump_activity(7);
    heap.print_data();
    // heap_pretty_print(heap);

    std::cout << "Removing variable 2.\n\n";
    heap.remove(2);
    heap.print_data();

    std::cout << "Removing variable with most activity, ";
    int v_max = heap.removeMax();
    std::cout << v_max << ".\n\n";
    heap.print_data();

    return 0;
}