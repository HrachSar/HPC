#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

#define N 24000000

__global__ void convert_gpu(char *d_in){
    unsigned int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid >= N)
        return;

    int codon_id = tid / 3;
    int warp_id = threadIdx.x / 32;
    int lane_id = threadIdx.x % 32;

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
    int block_size = 64;

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

    int gridSize = (N + block_size - 1) / block_size;
    cudaEventRecord(start);
    convert_gpu<<<gridSize, block_size>>>(d_dna);
    cudaEventRecord(stop);

    err = cudaDeviceSynchronize();
    if(err != cudaSuccess){
        perror("Error on syncing device\n");
        return 1;
    }

    float elapsed = 0;
    cudaEventElapsedTime(&elapsed, start, stop);

    printf("Block Size: %d\n",block_size);
    printf("Grid Size: %d\n",gridSize);
    printf("Kernel execution time in ms: %lf\n", elapsed);
    printf("Total number of codons: %d\n", N / 3);
    printf("Total number of threads: %d\n", block_size * gridSize);
    printf("Total number of warps: %d\n", block_size * gridSize / 32);

    printf("\n");

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