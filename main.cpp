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

    // allocate and initialize large matrix
    std::vector<std::vector<int>> matrix(size, std::vector<int>(size, 0));
    {
        int value = 1;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                matrix[i][j] = value++;
            }
        }
    }

    std::vector<double> rowTimes, colTimes;
    rowTimes.reserve(iterations);
    colTimes.reserve(iterations);

    {
        long long dummySum = 0;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                dummySum += matrix[i][j];
    }

    for (int iterCount = 0; iterCount < iterations; ++iterCount) {
        // Row-major
        volatile long long rowSum = 0;
        auto startRow = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                rowSum += matrix[i][j];
            }
        }
        auto endRow = std::chrono::high_resolution_clock::now();
        rowTimes.push_back(std::chrono::duration<double>(endRow - startRow).count());

        // Col-major
        volatile long long colSum = 0;
        auto startCol = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                colSum += matrix[i][j];
            }
        }
        auto endCol = std::chrono::high_resolution_clock::now();
        colTimes.push_back(std::chrono::duration<double>(endCol - startCol).count());
    }

    double sumRow = 0.0;
    double sumCol = 0.0;
    for (double t : rowTimes) {
        sumRow += t;
    }
    for (double t : colTimes) {
        sumCol += t;
    }
    double avgRow = sumRow / iterations;
    double avgCol = sumCol / iterations;

    double stddevRow = 0.0;
    double stddevCol = 0.0;
    for (double t : rowTimes) {
        stddevRow += (t - avgRow) * (t - avgRow);
    }
    for (double t : colTimes) {
        stddevCol += (t - avgCol) * (t - avgCol);
    }
    stddevRow = std::sqrt(stddevRow / iterations);
    stddevCol = std::sqrt(stddevCol / iterations);

    std::cout << std::fixed << std::setprecision(9);
    std::cout << "Matrix size: " << size << " x " << size << "\n";
    std::cout << "Iterations: " << iterations << "\n\n";
    std::cout << "Average Row-major traversal time: " << avgRow << " seconds (stddev: " << stddevRow << ")\n";
    std::cout << "Average Column-major traversal time: " << avgCol << " seconds (stddev: " << stddevCol << ")\n\n";

    // -------------------------------------------------
    //  Test 2x3 with two different memory placements
    // -------------------------------------------------

    std::cout << "------------------------------------------------\n";
    std::cout << "Comparing traversal times for matrix size 2x3\n";
    std::cout << "------------------------------------------------\n";

    // 1) Aligned (normal) 2x3
    {
        std::vector<std::vector<int>> matrix2 = {{1,2,3},{4,5,6}};

        // Row-major pass
        volatile long long rowSum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                rowSum += matrix2[i][j];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double rowTimeAligned = std::chrono::duration<double>(end - start).count();

        // Col-major pass
        volatile long long colSum = 0;
        start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 2; ++i) {
                colSum += matrix2[i][j];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        double colTimeAligned = std::chrono::duration<double>(end - start).count();

        std::cout << "Case 1 (Aligned 2x3)\n";
        std::cout << "  Row-major time : " << rowTimeAligned << " seconds\n";
        std::cout << "  Col-major time : " << colTimeAligned << " seconds\n\n";
    }

    // 2) Purposely unaligned 2x3 (simulate crossing cache line boundary)
    {
        static std::vector<int> bigBuffer(64 + 6, 0);

        // Use an offset of 1 so it's almost guaranteed to misalign.
        int offset = 1;

        // 2D indexing manually:
        // row0: bigBuffer[offset + 0], bigBuffer[offset + 1], bigBuffer[offset + 2]
        // row1: bigBuffer[offset + 3], bigBuffer[offset + 4], bigBuffer[offset + 5]
        bigBuffer[offset + 0] = 1;
        bigBuffer[offset + 1] = 2;
        bigBuffer[offset + 2] = 3;
        bigBuffer[offset + 3] = 4;
        bigBuffer[offset + 4] = 5;
        bigBuffer[offset + 5] = 6;

        // Row-major pass
        volatile long long rowSum = 0;
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 3; ++j) {
                rowSum += bigBuffer[offset + i*3 + j];
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        double rowTimeUnaligned = std::chrono::duration<double>(end - start).count();

        // Col-major pass
        volatile long long colSum = 0;
        start = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 2; ++i) {
                colSum += bigBuffer[offset + i*3 + j];
            }
        }
        end = std::chrono::high_resolution_clock::now();
        double colTimeUnaligned = std::chrono::duration<double>(end - start).count();

        std::cout << "Case 2 (Unaligned 2x3)\n";
        std::cout << "  Row-major time : " << rowTimeUnaligned << " seconds\n";
        std::cout << "  Col-major time : " << colTimeUnaligned << " seconds\n";
    }

    return 0;
}
