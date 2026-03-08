********************************************************************************
Task 1: DNA Nucleotide Statistics using SIMD and Multithreading
Objective
The goal of this assignment is to practice:

    data-level parallelism using SIMD
    task-level parallelism using multithreading
    combining SIMD and multithreading
    parallel reduction / aggregation of partial results

You will implement several versions of a program that analyzes a large DNA sequence and counts how many times each nucleotide appears.

Problem Description

In bioinformatics, one of the basic tasks is to compute simple statistics for a DNA sequence.

A DNA sequence consists of the following nucleotide symbols: 'A', 'C', 'G', 'T'

Given a DNA sequence, write a program that counts how many times each of these symbols occurs.

The final output must contain four integers representing the number of occurrences of: A C G T

Example:

Input:
AGCTTTTCATTCTGACTGCAACGGGCAATATGTCTCTGTGTGGATTAAAAAAAGAGTGTCTGATAGCAGC

Output:
20 12 17 21

DNA Buffer Preparation

        The buffer must contain only the characters: 'A', 'C', 'G', 'T'
        The sequence size must be large enough for performance measurements (100MB, 256 MB, 512MB, ...)
        You may prepare the DNA sequence by generating it randomly.


Multithreading Implementation

        Use POSIX threads (pthread).
        Divide the DNA buffer into chunks.
        Each thread must process one chunk and compute in a global variable for each character. You need to protect it using a mutex or spinlock!!!

SIMD Implementation

        Use SIMD intrinsics available on your platform.
        Examples:

                SSE / AVX2 on x86

                NEON on ARM
        Process multiple characters in parallel.
        Compare vector elements against the four nucleotide symbols: 'A', 'C', 'G', 'T'
        Accumulate the counts correctly.
        Handle any remaining elements that do not fit into the SIMD vector width.

SIMD + Multithreading Implementation

        Divide the DNA buffer among multiple threads.
        Each thread must use SIMD to process its own chunk.
        Each thread must maintain local counts for: A, C, G, T
        After all threads finish, merge the partial counts into final totals.

Scalar Reference Version

        Process the DNA sequence one character at a time.
        Use this version as a reference implementation.
        Use it to verify correctness of the optimized versions.

Performance Measurement

        Scalar
        Multithreading
        SIMD
        SIMD + Multithreading

Use a high-resolution timer such as: clock_gettime()

Program Output

Your program should print results in a format similar to:

DNA size: 256 MB
Threads used: 4

Counts (A C G T):
67108864 67108864 67108864 67108864

Scalar time:                0.220 sec
Multithreading time:        0.095 sec
SIMD time:                  0.080 sec
SIMD + Multithreading time: 0.028 sec

********************************************************************************
Task 2: Character Buffer Processing using SIMD and Multithreading
Objective
The goal of this assignment is to practice:

    data-level parallelism using SIMD
    task-level parallelism using multithreading
    combining SIMD and multithreading for performance optimization

You will implement several versions of a program that processes a large buffer of characters and converts lowercase letters to uppercase.

Problem Description
Given a large buffer of characters, write a program that scans the buffer and converts every lowercase English letter into its uppercase equivalent.
Only characters in the range 'a' – 'z' must be converted. All other characters must remain unchanged.

Example:

Input:  "Hello world! a1b2"
Output: "HELLO WORLD! A1B2"

Requirements:

Your program must implement three different versions of the solution:

        Multithreading implementation

        SIMD implementation

        SIMD + Multithreading implementation

Each implementation must process the same input buffer.

Buffer Preparation: Allocate a buffer of at least: 200MB, fill the buffer with characters that include:

        lowercase letters

        uppercase letters

        digits

        punctuation

        spaces

Create separate copies of the buffer so each implementation works on identical data.

Multithreading Implementation

        Use POSIX threads (pthread).

        Divide the buffer into equal-sized chunks.

        Each thread processes one chunk of the buffer.

        Each thread converts lowercase letters to uppercase in its region.

SIMD Implementation

Use SIMD intrinsics available on your platform.

Examples:

        SSE / AVX2 (x86)

        NEON (ARM)

        Process multiple characters per iteration using vector registers.

        Detect lowercase ASCII letters and convert them to uppercase.

        Handle remaining characters that do not fit into the SIMD vector width.

SIMD + Multithreading Implementation

Implement a version that combines both techniques.

Requirements:

        Divide the buffer among multiple threads.

        Each thread processes its chunk using SIMD operations.

        Ensure that thread boundaries are handled correctly.

Performance Measurement: Measure the execution time of each implementation.

Your program should print results similar to:

Buffer size: 256 MB
Threads used: 4

Multithreading time:      0.145 sec
SIMD time:                0.098 sec
SIMD + Multithreading:    0.041 sec

********************************************************************************
