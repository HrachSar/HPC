#include <stdio.h>
#include <pthread.h>
#include <immintrin.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#define T 16

typedef struct Pixel{
	unsigned char r, g, b, pad;
}Pixel;

typedef struct Image{
	int width, height, max_color;
	Pixel *pixels;
}Image;


typedef struct thread_args{
	size_t start;
	size_t end;
} thread_args;

typedef struct timer{
	double time_sc;
	double time_mt;
	double time_simd;
	double time_mt_simd;
} timer;

Image *img;
char magic[3];

int read_ppm(const char *filename){
	FILE *f = fopen(filename, "rb");
	if(!f){
		perror("Error on opening a file.\n");
		return 1;
	}

	img = (Image *)malloc(sizeof(Image));
	if(!img){
		perror("Error on allocating memory for image.\n");
		return 1;
	}

	if(fscanf(f, "%2s", magic) != 1){
		perror("Error reading from file.\n");
		fclose(f);
		free(img);
		return 1;
	}

	fgetc(f);
	if(fscanf(f, "%d %d %d", &img->width, &img->height, &img->max_color) != 3){
		perror("Error reading from file.\n");
		fclose(f);
		free(img);
		return 1;
	}

	fgetc(f);

	img->pixels = (Pixel *)malloc(img->width * img->height * sizeof(Pixel));
	if(!img->pixels){
		perror("Error on allocating memory for pixels.\n");
		free(img);
		return 1;
	}

	for(int i = 0; i < img->width * img->height; i++){
	    int r, g, b;
	    fscanf(f, "%d %d %d", &r, &g, &b);

	    img->pixels[i].r = r;
	    img->pixels[i].g = g;
	    img->pixels[i].b = b;
	}
	fclose(f);
	return 0;
}

int write_ppm(const char *filename){
	FILE *f = fopen(filename, "wb");
	if(!f){
		perror("Error on opening a file.\n");
		return 1;
	}

	fprintf(f, "%2s\n", magic);
	fprintf(f, "%d %d\n", img->width, img->height);
	fprintf(f, "%d\n", img->max_color);

	for(int i = 0; i <  img->height; i++){
		for(int j = 0; j < img->width; j++){
			fprintf(f, "%d %d %d  ", img->pixels[i * img->width + j].r,
				img->pixels[i * img->width + j].g, img->pixels[i * img->width + j].b);
		}
		fprintf(f, "\n");
	}

	fclose(f);
	return 0;
}

void free_ppm(Image *i){
	if(i){
		free(i->pixels);
		free(i);
	}
}

void convert_scalar(Image *img){
	int i = 0, j = 0;

	for(i = 0; i < img->height; i++){
		for(j = 0; j < img->width; j++){
			int gray = .299 * img->pixels[i * img->width + j].r +
					   .587 * img->pixels[i * img->width + j].g +
					   .114 * img->pixels[i * img->width + j].b;
			img->pixels[i * img->width + j].r = gray;
			img->pixels[i * img->width + j].g = gray;
			img->pixels[i * img->width + j].b = gray;
		}
	}
}

void *convert_mt(void *args){
	thread_args *targs = (thread_args *)args;

	for(int i = targs->start; i < targs->end; i++){
		int gray = .299 * img->pixels[i].r +
				   .587 * img->pixels[i].g +
				   .114 * img->pixels[i].b;
		img->pixels[i].r = gray;
		img->pixels[i].g = gray;
		img->pixels[i].b = gray;
	}
	return NULL;
}

void convert_simd(Image *img){
	int i = 0;

	__m256 first = _mm256_set1_ps(0.299);
	__m256 second = _mm256_set1_ps(0.587);
	__m256 third = _mm256_set1_ps(0.114);

	for(; i < img->height * img->width; i += 8){
		__m256i va = _mm256_loadu_si256((__m256i *)&img->pixels[i]);

		__m256i mask = _mm256_set1_epi32(0xFF);

    	__m256i r = _mm256_and_si256(va, mask);
    	__m256i g = _mm256_and_si256(_mm256_srli_epi32(va, 8), mask);
    	__m256i b = _mm256_and_si256(_mm256_srli_epi32(va, 16), mask);

    	__m256 rf = _mm256_cvtepi32_ps(r);
    	__m256 gf = _mm256_cvtepi32_ps(g);
    	__m256 bf = _mm256_cvtepi32_ps(b);

    	__m256 grayf =
    	    _mm256_add_ps(
    	        _mm256_add_ps(_mm256_mul_ps(rf, first),
    	                      _mm256_mul_ps(gf, second)),
    	        _mm256_mul_ps(bf, third)
    	    );

    	__m256i gray = _mm256_cvtps_epi32(grayf);
		int gray_vals[8];
		int k = 0;
		_mm256_storeu_si256((__m256i*)gray_vals, gray);
		for(; k < 8; k++){
    		img->pixels[i+k].r = gray_vals[k];
    		img->pixels[i+k].g = gray_vals[k];
    		img->pixels[i+k].b = gray_vals[k];
		}
	}
}

void *convert_simd_mt(void *args){
	thread_args *targs = (thread_args *)args;

	__m256 first = _mm256_set1_ps(0.299);
	__m256 second = _mm256_set1_ps(0.587);
	__m256 third = _mm256_set1_ps(0.114);
	int i = targs->start;
	for(; i < targs->end; i += 8){
		__m256i va = _mm256_loadu_si256((__m256i *)&img->pixels[i]);

		__m256i mask = _mm256_set1_epi32(0xFF);

    	__m256i r = _mm256_and_si256(va, mask);
    	__m256i g = _mm256_and_si256(_mm256_srli_epi32(va, 8), mask);
    	__m256i b = _mm256_and_si256(_mm256_srli_epi32(va, 16), mask);

    	__m256 rf = _mm256_cvtepi32_ps(r);
    	__m256 gf = _mm256_cvtepi32_ps(g);
    	__m256 bf = _mm256_cvtepi32_ps(b);

    	__m256 grayf =
    	    _mm256_add_ps(
    	        _mm256_add_ps(_mm256_mul_ps(rf, first),
    	                      _mm256_mul_ps(gf, second)),
    	        _mm256_mul_ps(bf, third)
    	    );

    	__m256i gray = _mm256_cvtps_epi32(grayf);
		int gray_vals[8];
		int k = 0;
		_mm256_storeu_si256((__m256i*)gray_vals, gray);
		for(; k < 8; k++){
    		img->pixels[i+k].r = gray_vals[k];
    		img->pixels[i+k].g = gray_vals[k];
    		img->pixels[i+k].b = gray_vals[k];
		}
	}
	return NULL;
}

int main(){
	read_ppm("photo.ppm");


	struct timespec start, end;
	timer t;
	thread_args args[T];
	pthread_t threads[T];
	int N = img->height * img->width;

	Pixel *original = malloc(N * sizeof(Pixel));
	memcpy(original, img->pixels, N * sizeof(Pixel));

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
		if(pthread_create(&threads[i], NULL, convert_mt, &args[i]) != 0){
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
	memcpy(img->pixels, original, N * sizeof(Pixel));

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	convert_simd(img);
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }

	t.time_simd = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
	memcpy(img->pixels, original, N * sizeof(Pixel));

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
		if(pthread_create(&threads[i], NULL, convert_simd_mt, &args[i]) != 0){
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
	memcpy(img->pixels, original, N * sizeof(Pixel));

	if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	convert_scalar(img);
	if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime error");
        return 1;
    }
	t.time_sc = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;


	printf("IMAGE SIZE: %d x %d\n", img->width, img->height);
	printf("Threads used: %d\n", T);
	printf("\n");
	printf("Scalar time = %f\n", t.time_sc);
	printf("Multithreading time = %f\n", t.time_mt);
	printf("SIMD time = %f\n", t.time_simd);
	printf("SIMD + Multithreading time = %f\n", t.time_mt_simd);

	write_ppm("gray.ppm");
	free_ppm(img);
	return 0;
}

