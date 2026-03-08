#include <stdio.h>
#include <pthread.h>
#include <immintrin.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

#define N 200000000
#define T 16

char *buffer1;
char *buffer2;
char *buffer3;

typedef struct thread_args{
	size_t start;
	size_t end;
} thread_args;

typedef struct timer{
	double time_mt;
	double time_simd;
	double time_mt_simd;
} timer;

int is_lowercase(char c){
	if(c >= 'a' && c <= 'z')
		return 1;
	return 0;
}

void fill_buffer(char *buffer, size_t size){
	char pattern[] = "I love simd Programming!!! This is task number 2,"
						" I am SoLVing task Number 2.\0";

	size_t pattern_size = sizeof(pattern) - 1;
	for(size_t i = 0; i < size; i++){
		buffer[i] = pattern[i % pattern_size];
	}
}

void print_buffer(char *buffer, size_t size){
	for(size_t i = 0; i < size; i++){
		printf("%c ", buffer[i]);
	}
	printf("\n");
}

void *lower_to_upper_mt(void *args){

	thread_args *targs = (thread_args *)args;

	for(size_t i = targs->start; i < targs->end; i++){
		if(is_lowercase(buffer1[i])){
			buffer1[i] -= 32;
		}
	}

	return NULL;
}

void lower_to_upper_simd(char *buffer){

	size_t i = 0;
	__m256i a = _mm256_set1_epi8('a');
	__m256i z = _mm256_set1_epi8('z');
	__m256i sub = _mm256_set1_epi8(32);

	for(; i < N; i += 32){
		__m256i va = _mm256_loadu_si256((__m256i *)&buffer[i]);
		__m256i compareA = _mm256_cmpgt_epi8(va, _mm256_sub_epi8(a, _mm256_set1_epi8(1)));
		__m256i compareZ = _mm256_cmpgt_epi8(_mm256_add_epi8(z, _mm256_set1_epi8(1)), va);
		__m256i mask = _mm256_and_si256(compareA, compareZ);
		__m256i lower = _mm256_sub_epi8(va, _mm256_and_si256(mask, sub));

		_mm256_storeu_si256((__m256i *)&buffer[i], lower);
	}
}

void *lower_to_upper_simd_mt(void *args){
	thread_args *targs = (thread_args *)args;

	size_t limit = targs->start + ((targs->end - targs->start) & ~(32 - 1));
	size_t i = targs->start;
	__m256i a = _mm256_set1_epi8('a');
	__m256i z = _mm256_set1_epi8('z');
	__m256i sub = _mm256_set1_epi8(32);

	for(; i < limit; i += 32){
		__m256i va = _mm256_loadu_si256((__m256i *)&buffer3[i]);
		__m256i compareA = _mm256_cmpgt_epi8(va, _mm256_sub_epi8(a, _mm256_set1_epi8(1)));
		__m256i compareZ = _mm256_cmpgt_epi8(_mm256_add_epi8(z, _mm256_set1_epi8(1)), va);
		__m256i mask = _mm256_and_si256(compareA, compareZ);
		__m256i lower = _mm256_sub_epi8(va, _mm256_and_si256(mask, sub));

		_mm256_storeu_si256((__m256i *)&buffer3[i], lower);
	}

	for(; i < targs->end; i++){
		if(is_lowercase(buffer3[i])){
			buffer3[i] -= 32;
		}
	}
	return NULL;
}

int main(){

	struct timespec start, end;
	timer t;

	buffer1 = (char *)malloc(N);
	if(!buffer1){
		perror("Erron on allocating mem.\n");
		return 1;
	}
	buffer2 = (char *)malloc(N);
	if(!buffer2){
		perror("Erron on allocating mem.\n");
		return 1;
	}
	buffer3 = (char *)malloc(N);
	if(!buffer3){
		perror("Erron on allocating mem.\n");
		return 1;
	}

	fill_buffer(buffer1, N);
	fill_buffer(buffer2, N);
	fill_buffer(buffer3, N);


	thread_args args[T];
	pthread_t threads[T];

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }

	for(int i = 0; i < T; i++){
		args[i].start = i * (N / T);
		if(i < T - 1){
			args[i].end = (i + 1) * (N / T);
		}else{
			args[i].end = N;
		}
		if(pthread_create(&threads[i], NULL, lower_to_upper_mt, &args[i]) != 0){
			perror("Error on creating threads.\n");
			return 1;
		}
	}
	for(int i = 0; i < T; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_mt = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	lower_to_upper_simd(buffer2);
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_simd = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }

	for(int i = 0; i < T; i++){
		args[i].start = i * (N / T);
		if(i < T - 1){
			args[i].end = (i + 1) * (N / T);
		}else{
			args[i].end = N;
		}
		if(pthread_create(&threads[i], NULL, lower_to_upper_simd_mt, &args[i]) != 0){
			perror("Error on creating threads.\n");
			return 1;
		}
	}
	for(int i = 0; i < T; i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("Error on joining threads.\n");
			return 1;
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_mt_simd = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

	printf("BUFFER SIZE: %d MB\n", N / (1000 * 1000));
	printf("Threads used: %d\n", T);
	printf("\n");
	printf("Multithreading time = %f\n", t.time_mt);
	printf("SIMD time = %f\n", t.time_simd);
	printf("SIMD + Multithreading time = %f\n", t.time_mt_simd);

	free(buffer1);
	free(buffer2);
	free(buffer3);
	return 0;
}
