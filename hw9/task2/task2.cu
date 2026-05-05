#include <stdio.h>
#include <cuda_runtime.h>
#include <stdlib.h>

#define N 1000

__global__ void replace_occurances(char *dev){

    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if(idx >= N)
        return; 

    if(dev[idx] == 'T')
        dev[idx] += 1;
    
    __syncthreads();    

}


void generate_dna(char *arr){
    char types[4] = {'A', 'G', 'C', 'T'};

    for(int i = 0; i < N; i++){
        arr[i] = types[rand() % 4];
    }
    arr[N] = '\0';
}

int main(){

    srand(11);
    char *h_a, *d_a;
    h_a = (char *)malloc(sizeof(char) * (N + 1));
    if(!h_a){
        perror("Error on allocating memory for the host.\n");
        return 1;
    }
    generate_dna(h_a);

    printf("Old string = %s\n", h_a);


    cudaError_t err = cudaMalloc((void **)&d_a, (N + 1) * sizeof(char));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpy(d_a, h_a, (N + 1) * sizeof(char), cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    dim3 dimBlock(256);
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x);

    replace_occurances<<<dimGrid, dimBlock>>>(d_a);

    cudaDeviceSynchronize();

    err = cudaMemcpy(h_a, d_a, (N + 1) * sizeof(char), cudaMemcpyDeviceToHost);
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }    

    printf("New string = %s\n", h_a);

    free(h_a);
    cudaFree(d_a);
    return 0;
}