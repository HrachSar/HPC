Assignment 1: Parallel DNA Complement Generation
Objective
Implement a CUDA program that processes a large DNA sequence in parallel and generates its complementary sequence.

Background
DNA sequences consist of four nucleotides:

A
C
G
T
The complementary pairing rules are:

A → T
T → A
C → G
G → C
Task
Write a CUDA program that:

Generates a DNA sequence of length: N = 10,000,000
Stores the sequence in a 1D array using random values from {A, C, G, T}
Copies the sequence to the GPU
Launches a CUDA kernel where each thread:
computes its global thread ID
checks bounds
reads one nucleotide
writes its complement into an output array
Copies the result back to the CPU
Experiments
Run the program with block sizes:

64
128
256
512
Output Requirements
For each run, print:

First 50 elements of the original sequence
First 50 elements of the complemented sequence
Total number of threads launched
Kernel execution time
Assignment 2: Parallel DNA Base Frequency Analysis
Objective
Analyze a large DNA sequence in parallel and compute the frequency of each nucleotide using CUDA.

Background
A common task in bioinformatics is counting how many times each nucleotide appears in a sequence:

number of A
number of C
number of G
number of T
Task
Write a CUDA program that:

Generates a DNA sequence of length: N = 20,000,000
Fills it with random values from {A, C, G, T}
Copies the data to the GPU
Launches a CUDA kernel where each thread:
computes its global thread ID
checks bounds
reads one nucleotide
updates the corresponding counter
Kernel Requirement
Use four global counters for:

A
C
G
T
Use atomic operations to update counters.

Experiments
Run the program with block sizes:

64
128
256
512
Output Requirements
For each run, print:

Count of A
Count of C
Count of G
Count of T
Total sum of all counts
Kernel execution time
Block size and number of blocks
Assignment 3: Codon and Warp Mapping Analysis
Objective
Investigate how biological data grouping (codons) maps onto CUDA execution grouping (warps).

Background
DNA is interpreted in groups of three nucleotides called codons.

Example: ATG CGA TTT ...

Task
Write a CUDA program that:

Generates a DNA sequence of length: N = 24,000,000 (guaranteed to be divisible by 3)
Copies the sequence to the GPU
Launches a CUDA kernel where each thread:
computes global thread ID
checks bounds
determines its codon ID: int codon_id = gid / 3;
determines warp and lane:
int warp_id = threadIdx.x / 32;
int lane_id = threadIdx.x % 32;
Required Output
For the full dataset, compute and print:

Total number of codons
Total number of threads
Total number of warps