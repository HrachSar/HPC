#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10000000

__global__ void convert_gpu(char *d_in, char *d_out){
    unsigned int tid = blockDim.x * blockIdx.x + threadIdx.x;

    if(tid >= N)
        return;

    switch (d_in[tid])
    {
    case 'A':
        d_out[tid] = 'T';
        break;
    case 'T':
        d_out[tid] = 'A';
        break;
    case 'G':
        d_out[tid] = 'C';
        break;
    default:
        d_out[tid] = 'G';
        break;
    }
}

void init_dna(char *h_dna){
    char types[4] = {'A', 'C', 'G', 'T'};

    for(size_t i = 0; i < N; i++){
        h_dna[i] = types[rand() % 4];
    }
}

int main(){

    char *h_dna, *d_dna, *d_out, *h_out;
    srand(42);
    int block_sizes[4] = {64, 128, 256, 512};

    h_dna = (char *)malloc(N);
    if(!h_dna){
        perror("Error allocating memory on the host side.\n");
        return 1;
    }
    init_dna(h_dna);

    h_out = (char *)malloc(N);
    if(!h_out){
        perror("Error allocating memory on the host side.\n");
        return 1;
    }

    cudaError_t err = cudaMalloc(((void **)&d_dna), N);
    if(err != cudaSuccess){
        perror("Error allocating memory on the device side.\n");
        return 1;
    }

    err = cudaMalloc(((void **)&d_out), N);
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
        int gridSize = (N + block_sizes[i] - 1) / block_sizes[i];
        cudaEventRecord(start);
        convert_gpu<<<gridSize, block_sizes[i]>>>(d_dna, d_out);
        cudaEventRecord(stop);

        err = cudaDeviceSynchronize();
        if(err != cudaSuccess){
            perror("Error on syncing device\n");
            return 1;
        }

        err = cudaMemcpy(h_out, d_out, N, cudaMemcpyDeviceToHost);
        if(err != cudaSuccess){
            perror("Error on copying from device to host");
            return 1;
        }

        float elapsed = 0;
        cudaEventElapsedTime(&elapsed, start, stop);
        printf("Block Dimension = %d\n", block_sizes[i]);
        printf("First 50 elements of the original sequence:\n");
        for(int j = 0; j < 50; j++){
            printf("In[%d] = %c\n", j, h_dna[j]);
        }
        printf("First 50 elements of the complemented sequence:\n");
        for(int j = 0; j < 50; j++){
            printf("Out[%d] = %c\n", j, h_out[j]);
        }
        printf("Total number of threads launched: %d\n", gridSize * block_sizes[i]);
        printf("Kernel execution time in ms: %lf\n", elapsed);

        err = cudaMemset(d_out, 0, N);
        if(err != cudaSuccess){
            perror("Error on memset\n");
            return 1;
        }
    }

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
    free(h_dna);
    free(h_out);
    err = cudaFree(d_dna);
    if(err != cudaSuccess){
        perror("Error on freeing device memory\n");
        return 1;
    }
    err = cudaFree(d_out);
    if(err != cudaSuccess){
        perror("Error on freeing device memory\n");
        return 1;
    }
    return 0;
}