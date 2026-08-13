/*
* This file is the driver file for testing the algorithmic complexities
* used in Big-O analysis.
*/

#include <random>
#include "BenchmarkTimer.hpp"
#include "SortingAlgorithms.hpp"

vector<int> randNumGen(int size) {
    /*
    * The function was created using an online resource, adjusted for this project. Visit
    * geeksforgeeks.org/cpp/how-to-generate-a-vector-with-random-values-in-c/ for more
    * information on the implementation details here.
    */
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000000);

    vector<int> V(size);
    for (int i = 0; i < size; i++) {
        V[i] = dis(gen);
    }
    return V;
}

int main() {
    // 1. Experiment parameters - 20 trials at each input size
    vector<int> inputSizes = {1000, 2500, 5000, 10000};
    const int K_TRAILS = 20;

    // 2. Instantiate Logger and Sorting Algorithm objects
    BenchmarkLogger logger;
    SortingAlgorithms sorter;

    cout << "Starting benchmarking experiment across " << inputSizes.size()
        << " input sizes with " << K_TRAILS << " trials each...\n\n";

    // 3. Driver loop (N input size x K trials)
    for (auto N : inputSizes){
        cout << "Running " << K_TRAILS << " trials for N = " << N << "...\n";

        for (int k = 0; k < K_TRAILS; k++) {
            vector<int> masterV = randNumGen(N);

            {
                // INSERTION SORT ALGORITHM
                auto data = masterV;
                BenchmarkTimer timer("insertion_sort", N, logger);
                sorter.insertionSort(data);
            }

            {
                // BUBBLE SORT ALGORITHM
                auto data = masterV;
                BenchmarkTimer timer("bubble_sort", N, logger);
                sorter.bubbleSort(data);
            }

            {
                // QUICK SORT ALGORITHM
                auto data = masterV;
                BenchmarkTimer timer("quick_sort", N, logger);
                sorter.quickSort(data);
            }

            {
                // STD SORT ALGORITHM
                auto data = masterV;
                BenchmarkTimer timer("std_sort", N, logger);
                sorter.stdSort(data);
            }
        }
    }

    cout << "\nBenchmark complete! Data metrics have been recorded to logs/benchmarks.log\n";

    return 0;
}