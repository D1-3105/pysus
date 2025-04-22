#include "load_libcuda.h"

#include <dlfcn.h>
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <cuda.h>

static void* real_libcuda = nullptr;
static std::once_flag libcuda_loaded;

static void ensure_real_libcuda_loaded() {
    std::call_once(libcuda_loaded, []() {
        const char* msg = "[🔥 LIBCUDA WRAP LOADED]\n";

        size_t msg_len = 0;
        while (msg[msg_len] != '\0') {
            ++msg_len;
        }
        write(STDERR_FILENO, msg, msg_len);

        const char* libcuda_path = "libcuda_real.so.1";
        real_libcuda = dlopen(libcuda_path, RTLD_LAZY | RTLD_GLOBAL);
        if (!real_libcuda) {
            fprintf(stderr, "[‼️] Failed to load real libcuda.so.1 from %s: %s\n", libcuda_path, dlerror());
            _exit(1);
        }
    });
}

__attribute__((constructor))
static void libcuda_wrap_ctor() {
    ensure_real_libcuda_loaded();
}

template<typename T>
T resolve(const char* name) {
    ensure_real_libcuda_loaded();
    void* sym = dlsym(real_libcuda, name);
    if (!sym) {
        fprintf(stderr, "[‼️] dlsym failed for %s: %s\n", name, dlerror());
    }
    return reinterpret_cast<T>(sym);
}


extern "C" __attribute__((visibility("default")))
CUresult cuInit(unsigned int Flags) {
    static auto real = resolve<CUresult(*)(unsigned int)>("cuInit");
    std::cout << "[🟢 cuInit intercepted]\n";
    return real ? real(Flags) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuMemAlloc(CUdeviceptr* dptr, size_t bytesize) {
    static auto real = resolve<CUresult(*)(CUdeviceptr*, size_t)>("cuMemAlloc");
    static int calls = 0;
    if (calls++ < 10) std::cerr << "[🟢 cuMemAlloc intercepted: " << bytesize << " bytes]\n";
    return real ? real(dptr, bytesize) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuMemFree(CUdeviceptr dptr) {
    static auto real = resolve<CUresult(*)(CUdeviceptr)>("cuMemFree");
    static int calls = 0;
    if (calls++ < 10) std::cerr << "[🟢 cuMemFree intercepted]\n";
    return real ? real(dptr) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuDeviceGetCount(int* count) {
    static auto real = resolve<CUresult(*)(int*)>("cuDeviceGetCount");
    std::cerr << "[🟢 cuDeviceGetCount intercepted]\n";
    return real ? real(count) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuDeviceGetName(char* name, int len, CUdevice dev) {
    static auto real = resolve<CUresult(*)(char*, int, CUdevice)>("cuDeviceGetName");
    std::cerr << "[🟢 cuDeviceGetName intercepted]\n";
    return real ? real(name, len, dev) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuDeviceGetAttribute(int* pi, CUdevice_attribute attrib, CUdevice dev) {
    static auto real = resolve<CUresult(*)(int*, CUdevice_attribute, CUdevice)>("cuDeviceGetAttribute");
    std::cerr << "[🟢 cuDeviceGetAttribute intercepted]\n";
    return real ? real(pi, attrib, dev) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuCtxCreate(CUcontext* pctx, unsigned int flags, CUdevice dev) {
    static auto real = resolve<CUresult(*)(CUcontext*, unsigned int, CUdevice)>("cuCtxCreate");
    std::cerr << "[🟢 cuCtxCreate intercepted]\n";
    return real ? real(pctx, flags, dev) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuMemGetInfo(size_t* free, size_t* total) {
    static auto real = resolve<CUresult(*)(size_t*, size_t*)>("cuMemGetInfo");
    std::cerr << "[🟢 cuMemGetInfo intercepted]\n";
    return real ? real(free, total) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuMemcpyHtoD(CUdeviceptr dstDevice, const void* srcHost, size_t ByteCount) {
    static auto real = resolve<CUresult(*)(CUdeviceptr, const void*, size_t)>("cuMemcpyHtoD");
    std::cerr << "[🟢 cuMemcpyHtoD intercepted: " << ByteCount << " bytes]\n";
    return real ? real(dstDevice, srcHost, ByteCount) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuMemcpyDtoH(void* dstHost, CUdeviceptr srcDevice, size_t ByteCount) {
    static auto real = resolve<CUresult(*)(void*, CUdeviceptr, size_t)>("cuMemcpyDtoH");
    std::cerr << "[🟢 cuMemcpyDtoH intercepted: " << ByteCount << " bytes]\n";
    return real ? real(dstHost, srcDevice, ByteCount) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuGetErrorString(CUresult error, const char** pStr) {
    static auto real = resolve<CUresult(*)(CUresult, const char**)>("cuGetErrorString");
    std::cerr << "[🟢 cuGetErrorString intercepted]\n";
    return real ? real(error, pStr) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuCtxGetCurrent(CUcontext* pctx) {
    static auto real = resolve<CUresult(*)(CUcontext*)>("cuCtxGetCurrent");
    std::cerr << "[🟢 cuCtxGetCurrent intercepted]\n";
    return real ? real(pctx) : CUDA_ERROR_UNKNOWN;
}

extern "C" CUresult cuDevicePrimaryCtxRetain(CUcontext* pctx, CUdevice dev) {
    static auto real = resolve<CUresult(*)(CUcontext*, CUdevice)>("cuDevicePrimaryCtxRetain");
    std::cerr << "[🟢 cuDevicePrimaryCtxRetain intercepted]\n";
    return real ? real(pctx, dev) : CUDA_ERROR_UNKNOWN;
}