#include <stdio.h>
#include <cuda_runtime.h>
#include <stdlib.h>

#define N 1000


__device__ int d_result;

void generate_dna(char *arr){
    char types[4] = {'A', 'T', 'C', 'G'};

    for(int i = 0; i < N; i++){
        arr[i] = types[rand() % 4];
    }
    arr[N] = '\0';
}

__global__ void compare_dna(char *dev_a1, char *dev_a2){
    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    __shared__ int sdata[256];
    sdata[threadIdx.x] = 0;
    
    if(idx < N){
        if(dev_a1[idx] != dev_a2[idx])
            sdata[threadIdx.x] = 1;
    }

    __syncthreads();

    for(int stride = blockDim.x / 2; stride > 0; stride /= 2){
        if(threadIdx.x < stride)
            sdata[threadIdx.x] += sdata[threadIdx.x + stride];

        __syncthreads(); 
    }

    if(threadIdx.x == 0)
        atomicAdd(&d_result, sdata[0]);
}


int main(){

    srand(11);
    char *h_a1, *d_a1;
    char *h_a2, *d_a2;

    h_a1 = (char *)malloc(sizeof(char) * (N + 1));
    if(!h_a1){
        fprintf(stderr, "Failed to allocate host memory\n");
        return -1;
    }

    generate_dna(h_a1);
    
    cudaError_t err = cudaMalloc((void **)&d_a1, sizeof(char) * (N + 1));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory\n");
        return -1;
    }
    err = cudaMemcpy(d_a1, h_a1, sizeof(char) * (N + 1), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data to device\n");
        return -1;
    }

    h_a2 = (char *)malloc(sizeof(char) * (N + 1));
    if(!h_a2){
        fprintf(stderr, "Failed to allocate host memory\n");
        return -1;
    }

    generate_dna(h_a2);

    err = cudaMalloc((void **)&d_a2, sizeof(char) * (N + 1));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to allocate device memory\n");
        return -1;
    }
    err = cudaMemcpy(d_a2, h_a2, sizeof(char) * (N + 1), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy data to device\n");
        return -1;
    }

    int h_result = 0;

    err = cudaMemcpyToSymbol(d_result, &h_result, sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy result to device symbol\n");
        return -1;
    }

    dim3 dimBlock(256);
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x);

    compare_dna<<<dimGrid, dimBlock>>>(d_a1, d_a2);

    cudaDeviceSynchronize();
    err = cudaMemcpyFromSymbol(&h_result, d_result, sizeof(int));
    if (err != cudaSuccess) {
        fprintf(stderr, "Failed to copy result from device symbol\n");
        return -1;
    }

    printf("DNA Sequence 1: %s\n", h_a1);
    printf("DNA Sequence 2: %s\n", h_a2);
    printf("Number of differences between the two DNA sequences: %d\n", h_result);

    free(h_a1);
    free(h_a2);
    cudaFree(d_a1);
    cudaFree(d_a2);

    return 0;
}


