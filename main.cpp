#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>      
#include "kaizen.h"   

int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);

    // Check for required arguments: --size and --iterations.
    if (!args.is_present("--size") || !args.is_present("--iterations")) {
        std::cerr << "Usage: " << argv[0] 
                  << " --size <matrix_dimension> --iterations <num_iterations>" 
                  << std::endl;
        return 1;
    }

    int size = std::stoi(args.get_options("--size")[0]);
    int iterations = std::stoi(args.get_options("--iterations")[0]);
    if (size <= 0 || iterations <= 0) {
        std::cerr << "Matrix size and iterations must be positive integers." 
                  << std::endl;
        return 1;
    }

    // Allocate and initialize the matrix with sequential values.
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
    int value = 1;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = value++;
        }
    }

    // Vectors to store timings for each iteration.
    std::vector<double> rowTimes, colTimes;
    long long rowSumTotal = 0, colSumTotal = 0;

    // Optional warm-up run to reduce timing variability.
    {
        long long dummySum = 0;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                dummySum += matrix[i][j];
    }

    // Perform several tests.
    for (int iter = 0; iter < iterations; ++iter) {
        // --- Row-major order traversal ---
        long long rowSum = 0;
        auto startRow = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                rowSum += matrix[i][j];
            }
        }
        auto endRow = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> rowDuration = endRow - startRow;
        rowTimes.push_back(rowDuration.count());
        rowSumTotal += rowSum;
        
        // --- Column-major order traversal ---
        long long colSum = 0;
        auto startCol = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                colSum += matrix[i][j];
            }
        }
        auto endCol = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> colDuration = endCol - startCol;
        colTimes.push_back(colDuration.count());
        colSumTotal += colSum;
    }

    // Compute average times.
    double avgRow = 0, avgCol = 0;
    for (double t : rowTimes) { avgRow += t; }
    avgRow /= rowTimes.size();
    for (double t : colTimes) { avgCol += t; }
    avgCol /= colTimes.size();

    // Compute standard deviations.
    double stddevRow = 0, stddevCol = 0;
    for (double t : rowTimes) { stddevRow += (t - avgRow) * (t - avgRow); }
    stddevRow = std::sqrt(stddevRow / rowTimes.size());
    for (double t : colTimes) { stddevCol += (t - avgCol) * (t - avgCol); }
    stddevCol = std::sqrt(stddevCol / colTimes.size());

    // Output results.
    std::cout << "Matrix size: " << size << " x " << size << "\n";
    std::cout << "Iterations: " << iterations << "\n\n";
    std::cout << "Average Row-major traversal time: " << avgRow 
              << " seconds (stddev: " << stddevRow << ")\n";
    std::cout << "Average Column-major traversal time: " << avgCol 
              << " seconds (stddev: " << stddevCol << ")\n";

    return 0;
}
