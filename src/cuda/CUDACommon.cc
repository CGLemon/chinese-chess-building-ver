/*
    This file is part of ElephantArt.
    Copyright (C) 2021 Hung-Zhe Lin

    ElephantArt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ElephantArt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ElephantArt.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_CUDA

#include "cuda/CUDACommon.h"
#include "Utils.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace CUDA {

const char* cublasGetErrorString(cublasStatus_t status) {
    switch(status) {
        case CUBLAS_STATUS_NOT_SUPPORTED: return "CUBLAS_STATUS_NOT_SUPPORTED";
        case CUBLAS_STATUS_LICENSE_ERROR: return "CUBLAS_STATUS_LICENSE_ERROR";
        case CUBLAS_STATUS_SUCCESS: return "CUBLAS_STATUS_SUCCESS";
        case CUBLAS_STATUS_NOT_INITIALIZED: return "CUBLAS_STATUS_NOT_INITIALIZED";
        case CUBLAS_STATUS_ALLOC_FAILED: return "CUBLAS_STATUS_ALLOC_FAILED";
        case CUBLAS_STATUS_INVALID_VALUE: return "CUBLAS_STATUS_INVALID_VALUE"; 
        case CUBLAS_STATUS_ARCH_MISMATCH: return "CUBLAS_STATUS_ARCH_MISMATCH"; 
        case CUBLAS_STATUS_MAPPING_ERROR: return "CUBLAS_STATUS_MAPPING_ERROR";
        case CUBLAS_STATUS_EXECUTION_FAILED: return "CUBLAS_STATUS_EXECUTION_FAILED"; 
        case CUBLAS_STATUS_INTERNAL_ERROR: return "CUBLAS_STATUS_INTERNAL_ERROR"; 
    }
    return "unknown error";
}

void CublasError(cublasStatus_t status) {
    if (status != CUBLAS_STATUS_SUCCESS) {
        const char *cause = cublasGetErrorString(status);
        auto err = std::ostringstream{};
        err << "CUBLAS error: " << cause;
        throw std::runtime_error(err.str());
    }
}

void CudaError(cudaError_t status) {
  if (status != cudaSuccess) {
        const char *cause = cudaGetErrorString(status);
        auto err = std::ostringstream{};
        err << "CUDA Error: " << cause;
        throw std::runtime_error(err.str());
  }
}

int get_devicecount() {
    int n = 0;
    cudaError_t status = cudaGetDeviceCount(&n);
    ReportCUDAErrors(status);
    return n;
}

int get_device(int n) {
    cudaError_t status = cudaGetDevice(&n);
    ReportCUDAErrors(status);
    return n;
}

#ifdef USE_CUDNN
void CudnnError(cudnnStatus_t status) {
    if (status != CUDNN_STATUS_SUCCESS) {
        const char *s = cudnnGetErrorString(status);
        std::cerr << "CUDA Error: " << s << "\n";
        exit(-1);
    }
}

cudnnHandle_t cudnn_handle(int n) {
    static int init[MAX_SUPPORT_GPUS] = {0};
    static cudnnHandle_t handle[MAX_SUPPORT_GPUS];
    int i = get_device(n);
    if(!init[i]) {
        cudnnCreate(&handle[i]);
        init[i] = 1;
    }
    return handle[i];
}
#endif

cublasHandle_t blas_handle(int n) {
    static int init[MAX_SUPPORT_GPUS] = {0};
    static cublasHandle_t handle[MAX_SUPPORT_GPUS];
    int i = get_device(n);
    if (!init[i]) {
        cublasCreate(&handle[i]);
        init[i] = 1;
    }
    return handle[i];
}

void CudaHandel::apply(int n) {
#ifdef USE_CUDNN
  cudnn_handel = cudnn_handle(n);
#endif
  cublas_handel = blas_handle(n);
}

bool is_using_cuDNN() {
#ifdef USE_CUDNN
    return true;
#else
    return false;
#endif
}

void output_spec(const cudaDeviceProp &sDevProp) {
    Utils::printf<Utils::AUTO>(" Device name: %s\n", sDevProp.name);
    Utils::printf<Utils::AUTO>(" Device memory(MiB): %zu\n", (sDevProp.totalGlobalMem/(1024*1024)));
    Utils::printf<Utils::AUTO>(" Memory per-block(KiB): %zu\n", (sDevProp.sharedMemPerBlock/1024));
    Utils::printf<Utils::AUTO>(" Register per-block(KiB): %zu\n", (sDevProp.regsPerBlock/1024));
    Utils::printf<Utils::AUTO>(" Warp size: %zu\n", sDevProp.warpSize);
    Utils::printf<Utils::AUTO>(" Memory pitch(MiB): %zu\n", (sDevProp.memPitch/(1024*1024)));
    Utils::printf<Utils::AUTO>(" Constant Memory(KiB): %zu\n", (sDevProp.totalConstMem/1024));
    Utils::printf<Utils::AUTO>(" Max thread per-block: %zu\n", sDevProp.maxThreadsPerBlock);
    Utils::printf<Utils::AUTO>(" Max thread dim: (%zu, %zu, %zu)\n", sDevProp.maxThreadsDim[0], sDevProp.maxThreadsDim[1], sDevProp.maxThreadsDim[2]);
    Utils::printf<Utils::AUTO>(" Max grid size: (%zu, %zu, %zu)\n", sDevProp.maxGridSize[0], sDevProp.maxGridSize[1], sDevProp.maxGridSize[2]);
    Utils::printf<Utils::AUTO>(" Clock: %zu(kHz)\n", (sDevProp.clockRate/1000));
    Utils::printf<Utils::AUTO>(" textureAlignment: %zu\n", sDevProp.textureAlignment);
}

void check_devices() {
    int devicecount = get_devicecount();
    if (devicecount == 0) {
        throw std::runtime_error("No CUDA device");
    }

    int cuda_version;
    cudaDriverGetVersion(&cuda_version);
    {
        const auto major = cuda_version/1000;
        const auto minor = (cuda_version - major * 1000)/10;
        Utils::printf<Utils::AUTO>("CUDA version: Major %zu, Minor %zu\n", major, minor);
    }


    Utils::printf<Utils::AUTO>("Using cuDNN: ");
    if (is_using_cuDNN()) {
        Utils::printf<Utils::AUTO>("Yes\n");
#ifdef USE_CUDNN
        const auto cudnn_version = cudnnGetVersion();
        const auto major = cudnn_version/1000;
        const auto minor = (cudnn_version -  major * 1000)/100;
        Utils::printf<Utils::AUTO>("cuDNN version: Major %zu, Minor %zu\n", major, minor);
#endif
    } else {
        Utils::printf<Utils::AUTO>("No\n");
    }

    Utils::printf<Utils::AUTO>("Number of CUDA devices: %zu\n", devicecount);
    for(int i = 0; i < devicecount; ++i) {
        Utils::printf<Utils::AUTO>("=== Device %zu ===\n", i);
        cudaDeviceProp sDeviceProp;
        cudaGetDeviceProperties(&sDeviceProp, i);
        output_spec(sDeviceProp);
    }
    Utils::printf<Utils::AUTO>("\n");
}
} // namespace CUDA

#endif
