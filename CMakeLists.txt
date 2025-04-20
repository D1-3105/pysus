cmake_minimum_required(VERSION 3.15)
project(pysus)

set(CMAKE_CXX_STANDARD 20)


find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(Threads REQUIRED)

message(STATUS "✔ Found Python interpreter: ${Python3_EXECUTABLE}")
message(STATUS "✔ Python version: ${Python3_VERSION}")
message(STATUS "✔ Python include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "✔ Python libraries: ${Python3_LIBRARIES}")

add_executable(pysus src/main.cpp src/ThreadHolder.cpp)


target_include_directories(pysus PRIVATE ${Python3_INCLUDE_DIRS})
target_link_libraries(pysus PRIVATE ${Python3_LIBRARIES} Threads::Threads)


set_target_properties(pysus PROPERTIES
        BUILD_RPATH ${Python3_LIBRARY_DIRS}
)
