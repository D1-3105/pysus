cmake_minimum_required(VERSION 3.15)
project(pysus)

set(CMAKE_CXX_STANDARD 20)

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(Threads REQUIRED)
find_package(CUDA QUIET)

message(STATUS "✔ Found Python interpreter: ${Python3_EXECUTABLE}")
message(STATUS "✔ Python version: ${Python3_VERSION}")
message(STATUS "✔ Python include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "✔ Python libraries: ${Python3_LIBRARIES}")

if (CUDA_FOUND)
    message(STATUS "✔ CUDA found at: ${CUDA_TOOLKIT_ROOT_DIR}")
    message(STATUS "✔ CUDA include dir: ${CUDA_INCLUDE_DIRS}")
    message(STATUS "✔ CUDA libraries: ${CUDA_LIBRARIES}")
else()
    message(WARNING "⚠ CUDA not found, building without GPU support.")
endif()

add_executable(pysus src/main.cpp src/ThreadHolder.cpp
        src/cuda/PointerMap.cpp
        src/cuda/PointerMap.h
        src/cuda/load_libcuda.cpp
        src/cuda/load_libcuda.h)
target_include_directories(pysus PRIVATE ${Python3_INCLUDE_DIRS})

if (CUDA_FOUND)
    target_link_libraries(pysus PRIVATE ${CUDA_LIBRARIES})

    # CUDA override library
    add_library(cuda_override SHARED src/cuda/load_libcuda.cpp)
    target_include_directories(cuda_override PUBLIC ${CUDA_INCLUDE_DIRS})
    target_link_libraries(cuda_override PUBLIC dl cuda /usr/local/cuda/include/cuda.h)
    set_target_properties(
            cuda_override PROPERTIES
            OUTPUT_NAME "cuda_override"
    )
    target_link_libraries(
            cuda_override PRIVATE
            -lcuda
    )
    message(STATUS "✔ CUDA override: Built")

endif()

target_link_libraries(pysus PRIVATE
        ${Python3_LIBRARIES}
        Threads::Threads
)

if (CUDA_FOUND)
    target_link_libraries(pysus PRIVATE ${CUDA_LIBRARIES})
endif()

set_target_properties(pysus PROPERTIES
        BUILD_RPATH ${Python3_LIBRARY_DIRS}
)

