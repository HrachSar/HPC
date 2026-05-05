#include <stdio.h>
#include <cuda_runtime.h>
#include <stdlib.h>

#define N 1000

__device__ int count[4]; 

__global__ void count_occurances(char *dev){

    __shared__ int local_count[4];

    if(threadIdx.x < 4)
        local_count[threadIdx.x] = 0;

    __syncthreads();

    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if(idx >= N)
        return; 

    if(dev[idx] == 'A')
        atomicAdd(&local_count[0], 1);
    else if(dev[idx] == 'G')
        atomicAdd(&local_count[1], 1);
    else if(dev[idx] == 'C')
        atomicAdd(&local_count[2], 1);
    else if(dev[idx] == 'T')
        atomicAdd(&local_count[3], 1);
    
    __syncthreads();    

    if(threadIdx.x < 4)
        atomicAdd(&count[threadIdx.x], local_count[threadIdx.x]); 
}


void generate_dna(char *arr){
    char types[4] = {'A', 'G', 'C', 'T'};

    for(int i = 0; i < N; i++){
        arr[i] = types[rand() % 4];
    }
}

int main(){

    srand(11);
    char *h_a, *d_a;
    int host_counter[4] = {0};   
    h_a = (char *)malloc(sizeof(char) * N);
    if(!h_a){
        perror("Error on allocating memory for the host.\n");
        return 1;
    }
    generate_dna(h_a);

    cudaError_t err = cudaMalloc((void **)&d_a, N * sizeof(char));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpy(d_a, h_a, N * sizeof(char), cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpyToSymbol(count, host_counter, 4 * sizeof(int));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    dim3 dimBlock(256);
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x);

    count_occurances<<<dimGrid, dimBlock>>>(d_a);

    cudaDeviceSynchronize();

    err = cudaMemcpyFromSymbol(host_counter, count, 4 * sizeof(int));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }   

    printf("A = %d, G = %d, C = %d, T = %d\n", host_counter[0], 
                host_counter[1], host_counter[2], host_counter[3]);

    free(h_a);
    cudaFree(d_a);
    return 0;
}