#include <iostream>
#include <cuda_runtime_api.h>
#include <cuda.h>

//#include "cuda_test.h"

// CUDA kernel
__global__ void cudaAdd(unsigned char* a, unsigned char* b, unsigned char* c, int size) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < size) {
        c[idx] = a[idx] + b[idx] + (a[idx] + b[idx])*(a[idx] + b[idx]);
    }
}
int main() {
    int deviceCount = 0;
    cudaGetDeviceCount(&deviceCount);

    if (deviceCount == 0) {
        std::cerr << "No GPUs detected." << std::endl;
        return 1;
    }

    for (int i = 0; i < deviceCount; ++i) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, i);

        std::cout << "GPU Device " << i << ": " << deviceProp.name << std::endl;
        std::cout << "  Compute Capability: " << deviceProp.major << "." << deviceProp.minor << std::endl;
        std::cout << "  Total Global Memory: " << deviceProp.totalGlobalMem << " bytes" << std::endl;
        std::cout << "  Multiprocessors: " << deviceProp.multiProcessorCount << std::endl;
        std::cout << "  Clock Rate: " << deviceProp.clockRate / 1000 << " MHz" << std::endl;
        std::cout << "  Memory Clock Rate: " << deviceProp.memoryClockRate / 1000 << " MHz" << std::endl;
        std::cout << "  Memory Bus Width: " << deviceProp.memoryBusWidth << " bits" << std::endl;
    }
    const int size = 1024*1024*1024;    // 10 MB
    unsigned char* a = new unsigned char[size];
    unsigned char* b = new unsigned char[size];
    unsigned char* d_a, *d_b, *d_c;

    // Allocate GPU memory
    cudaMalloc((void**)&d_a, size);
    cudaMalloc((void**)&d_b, size);
    cudaMalloc((void**)&d_c, size);
    // Copy data from host to GPU
    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

    int threadsPerBlock = 32;
    int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;
    
    for(int i = 0; i < 10; i++){
    cudaMemcpyAsync(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpyAsync(d_b, b, size, cudaMemcpyHostToDevice);
    cudaAdd<<<blocksPerGrid, threadsPerBlock>>>(d_a, d_b, d_c, size);

    std::cout << "Matrix multiplication completed." << std::endl;
    }
    // Free GPU memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    // Do something with the result in 'c'
    delete[] a;
    delete[] b;

    return 0;
}