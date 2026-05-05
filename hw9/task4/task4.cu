#include <stdio.h>
#include <cuda_runtime.h>
#include <stdlib.h>

#define N 1000

const char codons[64][4] = {
    "UUU", "UUC", "UUA", "UUG", "UCU", "UCC", "UCA", "UCG",
    "UAU", "UAC", "UAA", "UAG", "UGU", "UGC", "UGA", "UGG",
    "CUU", "CUC", "CUA", "CUG", "CCU", "CCC", "CCA", "CCG",
    "CAU", "CAC", "CAA", "CAG", "CGU", "CGC", "CGA", "CGG",
    "AUU", "AUC", "AUA", "AUG", "ACU", "ACC", "ACA", "ACG",
    "AAU", "AAC", "AAA", "AAG", "AGU", "AGC", "AGA", "AGG",
    "GUU", "GUC", "GUA", "GUG", "GCU", "GCC", "GCA", "GCG",
    "GAU", "GAC", "GAA", "GAG", "GGU", "GGC", "GGA", "GGG"
};

const char amino_acids[64] = {
    'F', 'F', 'L', 'L', 'S', 'S', 'S', 'S',
    'Y', 'Y', '*', '*', 'C', 'C', '*', 'W',
    'L', 'L', 'L', 'L', 'P', 'P', 'P', 'P',
    'H', 'H', 'Q', 'Q', 'R', 'R', 'R', 'R',
    'I', 'I', 'I', 'M', 'T', 'T', 'T', 'T',
    'N', 'N', 'K', 'K', 'S', 'S', 'R', 'R',
    'V', 'V', 'V', 'V', 'A', 'A', 'A', 'A',
    'D', 'D', 'E', 'E', 'G', 'G', 'G', 'G'
};

__constant__ char d_codons[64][4];
__constant__ char d_amino_acids[64];

void generate_rna(char *arr){
    char types[4] = {'A', 'U', 'C', 'G'};

    for(int i = 0; i < N; i++){
        arr[i] = types[rand() % 4];
    }
    arr[N] = '\0';
}

__device__ int dev_strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}


__global__ void rna_to_protein(char *dev_rna, char *dev_protein){
    int idx = blockDim.x * blockIdx.x + threadIdx.x;

    if(idx < N){
        char codon[4];
        codon[0] = dev_rna[3 * idx];
        codon[1] = dev_rna[3 * idx + 1];
        codon[2] = dev_rna[3 * idx + 2];
        codon[3] = '\0';

        for(int i = 0; i < 64; i++){
            if(dev_strcmp(codon, d_codons[i]) == 0){
                dev_protein[idx] = d_amino_acids[i];
                break;
            }
        }
    }
}


int main(){

    srand(11);
    char *h_a, *d_a, *h_protein, *d_protein;

    h_a = (char *)malloc(sizeof(char) * (3 * N));
    if(!h_a){
        perror("Error on allocating memory for the host.\n");
        return 1;
    }
    generate_rna(h_a);

    cudaError_t err = cudaMalloc((void **)&d_a, 3 * N * sizeof(char));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpy(d_a, h_a, 3 * N * sizeof(char), cudaMemcpyHostToDevice);
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpyToSymbol(d_codons, codons, 64 * 4 * sizeof(char));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMemcpyToSymbol(d_amino_acids, amino_acids, 64 * sizeof(char));
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    err = cudaMalloc((void **)&d_protein, (N + 1) * sizeof(char));
    if(err != cudaSuccess){ 
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    h_protein = (char *)malloc(sizeof(char) * (N + 1));
    if(!h_protein){
        perror("Error on allocating memory for the host.\n");
        return 1;
    }
    h_protein[N] = '\0';

    dim3 dimBlock(256);
    dim3 dimGrid((N + dimBlock.x - 1) / dimBlock.x);

    rna_to_protein<<<dimGrid, dimBlock>>>(d_a, d_protein);

    cudaDeviceSynchronize();

    err = cudaMemcpy(h_protein, d_protein, (N + 1) * sizeof(char), cudaMemcpyDeviceToHost);
    if(err != cudaSuccess){
        printf("%s" ,cudaGetErrorString(err));
        return 1;
    }

    int i = 0;
    for(; i < N; i++){
       if(h_protein[i] == '*') {
            h_protein[i] = '\0';
            break;
        }
    }

    printf("Protein sequence: %s\n", h_protein);

    free(h_a);
    free(h_protein);
    cudaFree(d_a);
    cudaFree(d_protein);
    return 0;
}

