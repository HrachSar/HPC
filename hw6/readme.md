Task 1: Parallel Histogram with Reduction
Problem

You are given an array of size N = 100,000,000 with values in range [0, 255].

Compute a histogram: hist[256] → number of occurrences of each value

Steps:

    Initialize: A[i] = rand() % 256;
    Implement in the same program:
        Naive parallel version (race condition expected)
        Version with critical
        Version using reduction

Hint: OpenMP supports array reduction (OpenMP 4.5+) #pragma omp parallel for reduction(+:hist[:256])


Task 2: Global Minimum Distance (Closest Pair)
Problem

You are given an array of N=50,000,000 floating-point values.

Compute the minimum absolute difference between any two consecutive elements:

min⁡ | A[i] − A[i−1] |

Steps:

    Initialize: A[i] = random double
    Parallelize the loop
    Use reduction(min: min_diff)


Task 3: Parallel Filtered Sum (Top-K Style)
Problem

Given an array A[N], compute: sum of all elements where A[i]>T

Where: T=0.8 × max(A)

Steps:

    Find max(A) using reduction
    Compute threshold T
    Compute sum of elements > T using reduction
