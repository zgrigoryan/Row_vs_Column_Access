#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <numeric> 
#include <iomanip>
#include "kaizen.h"

int main(int argc, char* argv[]) {
    zen::cmd_args args(argv, argc);

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

    std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
    int value = 1;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            matrix[i][j] = value++;
        }
    }

    std::vector<double> rowTimes, colTimes;
    long long rowSumTotal = 0, colSumTotal = 0;

    {
        long long dummySum = 0;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                dummySum += matrix[i][j];
    }

    for (int iter = 0; iter < iterations; ++iter) {
        volatile long long rowSum = 0;
        auto startRow = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                rowSum += matrix[i][j];
            }
        }
        auto endRow = std::chrono::high_resolution_clock::now();
        rowTimes.push_back(std::chrono::duration<double>(endRow - startRow).count());
        rowSumTotal += rowSum;

        volatile long long colSum = 0;
        auto startCol = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                colSum += matrix[i][j];
            }
        }
        auto endCol = std::chrono::high_resolution_clock::now();
        colTimes.push_back(std::chrono::duration<double>(endCol - startCol).count());
        colSumTotal += colSum;
    }

    double avgRow = 0, avgCol = 0;
    for (double t : rowTimes) { avgRow += t; }
    avgRow /= rowTimes.size();
    for (double t : colTimes) { avgCol += t; }
    avgCol /= colTimes.size();

    double stddevRow = 0, stddevCol = 0;
    for (double t : rowTimes) { stddevRow += (t - avgRow) * (t - avgRow); }
    stddevRow = std::sqrt(stddevRow / rowTimes.size());
    for (double t : colTimes) { stddevCol += (t - avgCol) * (t - avgCol); }
    stddevCol = std::sqrt(stddevCol / colTimes.size());

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Matrix size: " << size << " x " << size << "\n";
    std::cout << "Iterations: " << iterations << "\n\n";
    std::cout << "Average Row-major traversal time: " << avgRow 
              << " seconds (stddev: " << stddevRow << ")\n";
    std::cout << "Average Column-major traversal time: " << avgCol 
              << " seconds (stddev: " << stddevCol << ")\n";

    std::cout << "\n------------------------------------------------\n";
    std::cout << "Comparing traversal times for matrix size 2x3" << "\n";
    std::cout << "------------------------------------------------\n";
    
    std::vector<std::vector<int>> matrix2 = {{1, 2, 3}, {4, 5, 6}};
    std::vector<double> rowTimes2, colTimes2;

    for (int iter = 0; iter < iterations; ++iter) {
        volatile long long rowSum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                rowSum += matrix2[i][j];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        rowTimes2.push_back(std::chrono::duration<double>(end - start).count());

        volatile long long colSum = 0;
        start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 2; ++i) {
                colSum += matrix2[i][j];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        colTimes2.push_back(std::chrono::duration<double>(end - start).count());
    }

    double avgRow2 = std::accumulate(rowTimes2.begin(), rowTimes2.end(), 0.0) / iterations;
    double avgCol2 = std::accumulate(colTimes2.begin(), colTimes2.end(), 0.0) / iterations;

    std::cout << "2x3 Row-major traversal avg time: " << avgRow2 << " seconds\n";
    std::cout << "2x3 Column-major traversal avg time: " << avgCol2 << " seconds\n";
    
    return 0;
}
