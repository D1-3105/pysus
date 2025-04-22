//
// Created by root on 4/21/25.
//

#ifndef LOAD_LIBCUDA_H
#define LOAD_LIBCUDA_H
#define _GNU_SOURCE
#include <cuda.h>

extern "C" CUresult cuMemAlloc(CUdeviceptr* dptr, size_t bytesize);
extern "C" CUresult cuMemFree(CUdeviceptr dptr);
extern "C" CUresult cuDeviceGetCount(int* count);
extern "C" CUresult cuDeviceGetName(char* name, int len, CUdevice dev);
extern "C" CUresult cuDeviceGetAttribute(int* pi, CUdevice_attribute attrib, CUdevice dev);
extern "C" CUresult cuCtxCreate(CUcontext* pctx, unsigned int flags, CUdevice dev);
extern "C" CUresult cuMemGetInfo(size_t* free, size_t* total);
extern "C" CUresult cuMemcpyHtoD(CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount);
extern "C" CUresult cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount);
extern "C" CUresult cuGetErrorString(CUresult error, const char** pStr);
extern "C" CUresult cuCtxGetCurrent(CUcontext* pctx);
extern "C" CUresult cuDevicePrimaryCtxRetain(CUcontext* pctx, CUdevice dev);


#endif //LOAD_LIBCUDA_H
