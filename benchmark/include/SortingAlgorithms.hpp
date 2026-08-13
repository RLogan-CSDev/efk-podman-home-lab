#ifndef SORTING_ALGORITHMS_HPP
#define SORTING_ALGORITHMS_HPP

/*
* This file contains the modularized sorting functions, along with helper functions needed
* to facilitate easier algorithmic analysis.
*/

#include <vector>
#include <algorithm>

class SortingAlgorithms {
private:
    inline int findPivot(int i, int j) {
        // Helper function for middle element in Quick Sort Algorithm
        /*
        * The function implementation comes from the Data Structures and Algorithm Analysis
        * book authored by Clifford A. Shaffer from page 246. With the help of AI, I rewrote
        * the function to use a vector storing int data types instead of a template.
        */
        return (i + j) / 2;
    }

    inline int partition(std::vector<int>& V, int l, int r, int pivot) {
        // Helper function for moving records to appropriate partition in Quick Sort Algorithm
        /*
        * The function implementation comes from the Data Structures and Algorithm Analysis
        * book authored by Clifford A. Shaffer from page 247. With the help of AI, I rewrote
        * the function to use a vector storing int data types instead of a template.
        */
        do {
            while (V[++l] < pivot);                 // Moves l to the right
            while ((r > l) && (pivot < V[--r]));    // Moves r to the left
            std::swap(V[l], V[r]);                  // Swap out of place values
        } while (l < r);                            // Stop when they cross
        return l;             // Returns the first position in the right partition
    }

    void qSort(std::vector<int>& V, int i, int j) {
        // Helper function using recursion for the Quick Sort Algorithm
        /*
        * The function implementation comes from the Data Structures and Algorithm Analysis
        * book authored by Clifford A. Shaffer from page 246. With the help of AI, I rewrote
        * the function to use a vector storing int data types instead of a template.
        */
        if (j <= i) return;                     // Don't sort 0 or 1 element
        int pivotIndex = findPivot(i, j);       
        std::swap(V[pivotIndex], V[j]);         // Put pivot at the end
        
        // k will be the first position in the right subarray
        int k = partition(V, i - 1, j, V[j]);
        std::swap(V[k], V[j]);                  // Put pivot in place
        qSort(V, i, k - 1);         // Recursive call reordering elements
        qSort(V, k + 1, j);         // Recursive call reordering elements
    }

public:
    void insertionSort(std::vector<int>& V) {
        /*
        * The function implementation comes from the Data Structures and Algorithm Analysis
        * book authored by Clifford A. Shaffer from page 234. With the help of AI, I rewrote
        * the function to use a vector storing int data types instead of a template.
        */
        int n = static_cast<int>(V.size());
        for (int i = 1; i < n; i++) {
            for (int j = i; (j > 0) && (V[j] < V[j - 1]); j--) {
                std::swap(V[j], V[j - 1]);
            }
        }
    }

    void bubbleSort(std::vector<int>& V) {
        /*
        * The function implementation comes from the Data Structures and Algorithm Analysis
        * book authored by Clifford A. Shaffer from page 236. With the help of AI, I rewrote
        * the function to use a vector storing int data types instead of a template.
        */
        int n = static_cast<int>(V.size());
        for (int i = 0; i < n - 1; i++) {
            for (int j = n - 1; j > i; j--) {
                if (V[j] < V[j - 1]) {
                    std::swap(V[j], V[j - 1]);
                }
            }
        }
    }

    void quickSort(std::vector<int>& V) {
        /*
        * With the help of AI, I was able to write the Quick Sort Algorithm execution function here.
        */
        if (!V.empty()) {
            qSort(V, 0, static_cast<int>(V.size()) - 1);
        }
    }

    void stdSort(std::vector<int>& V) {
        /*
        * For more information about the implementation, visit cplusplus.com/reference/algorithm/sort
        */
        std::sort(V.begin(), V.end());
    }
};

#endif /* SORTING_ALGORITHMS_HPP */