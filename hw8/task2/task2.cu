#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

#define N 20000000
__device__ unsigned int counter1 = 0;
__device__ unsigned int counter2 = 0;
__device__ unsigned int counter3 = 0;
__device__ unsigned int counter4 = 0;

__global__ void convert_gpu(char *d_in){
    unsigned int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid >= N)
        return;

    char nucleotide = d_in[tid];
    if (nucleotide == 'A')      atomicAdd(&counter1, 1);
    else if (nucleotide == 'T') atomicAdd(&counter2, 1);
    else if (nucleotide == 'C') atomicAdd(&counter3, 1);
    else if (nucleotide == 'G') atomicAdd(&counter4, 1);
}

void init_dna(char *h_dna){
    char types[4] = {'A', 'C', 'G', 'T'};

    for(size_t i = 0; i < N; i++){
        h_dna[i] = types[rand() % 4];
    }
}

int main(){

    char *h_dna, *d_dna;
    srand(42);
    int block_sizes[4] = {64, 128, 256, 512};

    h_dna = (char *)malloc(N);
    if(!h_dna){
        perror("Error allocating memory on the host side.\n");
        return 1;
    }
    init_dna(h_dna);

    cudaError_t err = cudaMalloc(((void **)&d_dna), N);
    if(err != cudaSuccess){
        perror("Error allocating memory on the device side.\n");
        return 1;
    }

    err = cudaMemcpy(d_dna, h_dna, N, cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
        perror("Error on copying from host to device");
        return 1;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    for(int i = 0; i < 4; i++){

        unsigned int zero = 0;
        cudaMemcpyToSymbol(counter1, &zero, sizeof(unsigned int));
        cudaMemcpyToSymbol(counter2, &zero, sizeof(unsigned int));
        cudaMemcpyToSymbol(counter3, &zero, sizeof(unsigned int));
        cudaMemcpyToSymbol(counter4, &zero, sizeof(unsigned int));

        int gridSize = (N + block_sizes[i] - 1) / block_sizes[i];
        cudaEventRecord(start);
        convert_gpu<<<gridSize, block_sizes[i]>>>(d_dna);
        cudaEventRecord(stop);

        err = cudaDeviceSynchronize();
        if(err != cudaSuccess){
            perror("Error on syncing device\n");
            return 1;
        }

        float elapsed = 0;
        cudaEventElapsedTime(&elapsed, start, stop);
        unsigned int h_counter1;
        unsigned int h_counter2;
        unsigned int h_counter3;
        unsigned int h_counter4;
        cudaMemcpyFromSymbol(&h_counter1, counter1, sizeof(unsigned int));
        cudaMemcpyFromSymbol(&h_counter2, counter2, sizeof(unsigned int));
        cudaMemcpyFromSymbol(&h_counter3, counter3, sizeof(unsigned int));
        cudaMemcpyFromSymbol(&h_counter4, counter4, sizeof(unsigned int));

        unsigned int count = h_counter1 + h_counter2 + h_counter3 + h_counter4;
        printf("Count of A = %u:\n", h_counter1);
        printf("Count of T = %u:\n", h_counter2);
        printf("Count of G = %u:\n", h_counter3);
        printf("Count of C = %u:\n", h_counter4);
        printf("Total count = %u\n", count);

        printf("Block Size: %d\n",block_sizes[i]);
        printf("Grid Size: %d\n",gridSize);
        printf("Kernel execution time in ms: %lf\n", elapsed);
        printf("\n");


    }

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    free(h_dna);
    err = cudaFree(d_dna);
    if(err != cudaSuccess){
        perror("Error on freeing device memory\n");
        return 1;
    }
    return 0;
}