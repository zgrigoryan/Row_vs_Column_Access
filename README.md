# Row-major vs Column-major Access 

This project demonstrates the performance difference between row-major and column-major traversal of a 2D matrix. The goal is to illustrate that row-major access is generally faster due to spatial locality and better cache utilization.

## Overview

The program initializes a square matrix with sequential values and performs two types of traversals:

- **Row-major Traversal:** Iterates row by row, accessing contiguous memory locations.
- **Column-major Traversal:** Iterates column by column, which is typically less cache-friendly.

To ensure robust statistics, the program runs each test over multiple iterations, calculates the average traversal time, and computes the standard deviation.

## Features

- **Matrix Initialization:** Creates an `n x n` matrix filled with sequential values.
- **Benchmarking:** Measures the execution time for both row-major and column-major traversals.
- **Statistical Analysis:** Calculates average time and standard deviation over a user-specified number of iterations.
- **Warm-up Run:** Includes a warm-up pass to reduce variability from initial cache and system overhead.
- **Command-Line Arguments:** Uses the [Kaizen library](https://github.com/heinsaar/kaizen) to parse command-line options (`--size` for matrix dimension and `--iterations` for the number of tests).

## Requirements

- A C++ compiler with C++11 support or later.

## Build Instructions

1. **Clone the Repository:**
   ```bash
   git clone https://https://github.com/zgrigoryan/Row_vs_Column_Access.git
   cd Row_vs_Column_Access
   ```
   
2. **Compile the Code:**
   For example, using `g++`:
   ```bash
   g++ -std=c++17 -O0 main.cpp -o matrix_benchmark
   ```

## Usage

Run the executable with the required command-line arguments:

```bash
./matrix_benchmark --size <matrix_dimension> --iterations <num_iterations>
```

For example, to benchmark a 1000x1000 matrix over 10 iterations:

```bash
./matrix_benchmark --size 1000 --iterations 10
```

The program will output:
- The average traversal times (in seconds) for both row-major and column-major accesses.
- The standard deviation for each traversal, indicating consistency.

## Explanation

- **Row-major Access:**  
  Iterating by rows takes advantage of the fact that elements in a row are stored contiguously in memory. This spatial locality leads to more efficient use of the CPU cache, which typically results in faster access times.

- **Column-major Access:**  
  Traversing by columns results in accessing non-contiguous memory locations. This pattern is less efficient due to poorer cache utilization, which is reflected in the longer execution times.

- **Statistical Testing:**  
  Running multiple iterations and calculating the average and standard deviation help smooth out any transient fluctuations in system performance, providing a more reliable measure of the relative performance of the two access patterns.

## License

This project is licensed under the MIT License. 

## Acknowledgments

- [Kaizen Library](https://github.com/heinsaar/kaizen) – A command-line argument parser used in this project.
