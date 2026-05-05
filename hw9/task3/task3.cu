#include <stdio.h>
#include <cuda_runtime.h>
#include <stdlib.h>

#define N 1000

const double vals[26] = {71.03711, 0, 103.00919, 115.02694, 
                129.04259, 147.06841, 57.02146, 137.05891, 
                113.08406, 0, 128.09496, 113.08406, 131.04049, 
                114.04293, 0, 97.05276, 128.05858, 156.10111, 
                87.03203, 101.04768, 0, 99.06841, 186.07931, 0, 163.06333, 0};

__constant__ double d_vals[26];                
__device__ double d_sum;


__global__ void compute_protein_mass(char *dev){

    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    __shared__ double sdata[256];

    double val = 0;

    if(idx < N)
        val = d_vals[dev[idx] - 'A'];

    sdata[threadIdx.x] = val;

    __syncthreads();

    for(int stride = blockDim.x / 2; stride > 0; stride /= 2){
        if(threadIdx.x < stride)
            sdata[threadIdx.x] += sdata[threadIdx.x + stride];

    __syncthreads(); 
    }
    
    if(threadIdx.x == 0)
        atomicAdd(&d_sum, sdata[0]);
   
}


void generate_protein(char *arr){
    char types[20] = {'A', 'C', 'D', 'E', 'F', 'G', 'H', 
                    'I', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y'};

    for(int i = 0; i < N; i++){
        arr[i] = types[rand() % 20];
    }
}

int main(){

    srand(11);
    char *h_a, *d_a;
    double h_sum = 0;
    h_a = (char *)malloc(sizeof(char) * N);
    if(!h_a){
        perror("Error on allocating memory for the host.\n");
        return 1;
    }
    generate_protein(h_a);

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

    err = cudaMemcpyToSymbol(d_vals, vals, 26 * sizeof(double));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpyToSymbol(d_sum, &h_sum, sizeof(double));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    dim3 dimBlock(256);
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x);

    compute_protein_mass<<<dimGrid, dimBlock>>>(d_a);

    cudaDeviceSynchronize();

    err = cudaMemcpyFromSymbol(&h_sum, d_sum, sizeof(double));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    printf("Total protein mass = %lf\n", h_sum);

    free(h_a);
    cudaFree(d_a);
    return 0;
}