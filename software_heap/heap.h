#ifndef HEAP_H
#define HEAP_H

#include <iostream>
#include <vector>

class Heap {
public:
    // Variable IDs go from 0 to n-1, where n is the number of variables
    
    std::vector<int> heap;
    std::vector<int> indices;
    std::vector<double> activity;

    // Maintain valid heap size
    int heap_size;

    // Increment, initialized to 1
    double increment;

    // Increment decay, which is 0.95
    double increment_decay;

    // Helper functions for heap
    static inline int left  (int i) { return i*2+1; }
    static inline int right (int i) { return (i+1)*2; }
    static inline int parent(int i) { return (i-1) >> 1; }

    // Percolate up helper function
    void percolateUp(int i) {
        int v = heap[i];
        int p_i = parent(i);
        int p = heap[p_i];

        while (i != 0 && activity[v] > activity[p]) {
            heap[i] = p;
            indices[p] = i;
            i = p_i;
            p_i = parent(p_i);
            p = heap[p_i];
        }

        heap[i] = v;
        indices[v] = i;
    }

    // Percolate down helper function
    void percolateDown(int i) {
        int v = heap[i];

        while (left(i) < heap_size) {
            int child = left(i);
            if (right(i) < heap_size && activity[heap[right(i)]] > activity[heap[left(i)]]) {
                child = right(i);
            }
            if (activity[heap[child]] <= activity[v]) {
                break;
            }
            heap[i] = heap[child];
            indices[heap[i]] = i;
            i = child;
        }

        heap[i] = v;
        indices[v] = i;
    }

    // Constructor accepts number of variables, all go into heap
    Heap(int max_num_vars) { 
        for (int i = 0; i < max_num_vars; ++i) {
            int v_id = i;

            // Add variable ID to heap
            heap.push_back(v_id);
            indices.push_back(i);

            // Initialize activity to 0
            activity.push_back(0);
        }

        heap_size = max_num_vars;
        increment = 1;
        increment_decay = 0.95;
    }

    int size() const {
        return heap_size;
    }
    bool empty() const {
        return heap_size == 0;
    }
    bool inHeap(int v) {
        return indices[v] >= 0;
    }

    // Call percolate up on a variable instead of index
    void percolateUpVar(int v) {
        int i = indices[v];
        percolateUp(i);
    }
    // Call percolate down on a variable instead of index
    void percolateDownVar(int v) {
        int i = indices[v];
        percolateDown(i);
    }

    // Insert variable into heap
    void insert(int v) {
        if(indices[v] >= 0) {
            std::cout << "Variable " << v << " already in heap" << std::endl;
            return;
        }

        indices[v] = heap_size;
        heap[heap_size] = v;
        heap_size++;

        percolateUp(indices[v]);
    }

    // Remove variable from heap
    void remove(int v) {
        if (indices[v] < 0) {
            std::cout << "Variable " << v << " already not in heap." << std::endl;
            return;
        }

        int i = indices[v];
        indices[v] = -1;
        heap_size--;

        if (i < heap_size) {
            heap[i] = heap.back();
            indices[heap[i]] = i;
            heap.pop_back();
            percolateDown(i);
        } else {
            heap.pop_back();
        }
    }

    // Remove variable with most activity
    int removeMax() {
        int v = heap[0];
        heap[0] = heap.back();
        indices[heap[0]] = 0;
        indices[v] = -1;
        heap.pop_back();

        heap_size--;

        if (heap_size > 1) {
            percolateDown(0);
        }

        return v;
    }



    // Activity-related functions
    // NOTE: does not have rescaling functionality
    void var_bump_activity(int v) {
        // Bump activity of variable
        activity[v] += increment;

        // Percolate up the variable if in heap
        if(indices[v] >= 0) {
            percolateUpVar(v);
        }
    }

    // Decay activity - achieved by multiplicatively increasing the increment
    void decay_activity() {
        // increment = increment / 0.95 = increment * 1.0526
        increment  = increment / increment_decay;
    }



    // Helper function to print data
    void print_data() {
        std::cout << "Heap (size = " << heap_size << "): ";
        for(int i = 0; i < heap_size; ++i) {
            std::cout << heap[i] << " ";
        }

        std::cout << "\nIndices: ";
        for(uint i = 0; i < indices.size(); ++i) {
            std::cout << indices[i] << " ";
        }
        std::cout << "\nActivities (increment = " << increment << "): ";
        for(uint i = 0; i < activity.size(); ++i) {
            std::cout << activity[i] << " ";
        }
        std::cout << "\n\n";
    }


};

#endif // HEAP_H