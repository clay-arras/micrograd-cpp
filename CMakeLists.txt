cmake_minimum_required(VERSION 3.10)
project(autograd_cpp LANGUAGES CXX CUDA)

set(Python_EXECUTABLE ".venv/bin/python")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -ffast-math -DNDEBUG") # bench
add_definitions(-DEIGEN_NO_DEBUG)

find_package(Eigen3 REQUIRED)
find_package(benchmark REQUIRED)
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)
find_package(CUDA REQUIRED)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF)
set(INSTALL_GTEST OFF)
set(BUILD_GMOCK ON)
FetchContent_MakeAvailable(googletest)

add_library(
  autograd_core
  autograd/engine/variable.cpp
  autograd/engine/tensor.cpp
  autograd/engine/storage.cpp
  autograd/engine/tensor_impl.cpp
  autograd/engine/tensor_helper.cpp
  autograd/engine/backend.cpp
  autograd/engine/constructor.cpp
  autograd/engine/backend/cuda_backend.cpp
  autograd/engine/functions/basic_ops.cpp
  autograd/engine/functions/unary_ops.cpp
  autograd/engine/functions/matrix_ops.cpp
  autograd/engine/functions/reduct_ops.cpp
  autograd/engine/functions/binary_ops.cpp
  autograd/engine/native/copy.cu
  autograd/engine/native/empty.cu
  autograd/engine/native/fill.cu
  autograd/engine/native/resize.cu
  autograd/util/csv_reader.cpp
  autograd/cuda/basic_kern.cu
  autograd/cuda/unary_kern.cu
  autograd/cuda/binary_kern.cu
  autograd/cuda/matrix_kern.cu
  autograd/cuda/reduct_kern.cu
)

target_compile_definitions(autograd_core PUBLIC)
target_link_libraries(autograd_core PUBLIC Eigen3::Eigen)

target_include_directories(
  autograd_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${EIGEN3_INCLUDE_DIRS}
                       ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
)

enable_testing()

# add_executable(mnist autograd/examples/mnist.cpp)
# target_link_libraries(mnist autograd_core)

# add_executable(bench_ops test/benchmarks/bench_ops.cpp)
# target_link_libraries(bench_ops autograd_core benchmark::benchmark)

add_executable(test_playground test/cpp/test_playground.cpp)
target_link_libraries(test_playground autograd_core)

# add_executable(test_debug test/python/stress/test_debug.cpp)
# target_link_libraries(test_debug autograd_core GTest::gtest_main)

add_executable(gtest_debug test/python/stress/gtest_debug.cpp)
target_link_libraries(gtest_debug autograd_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(gtest_debug)

pybind11_add_module(cpp_custom_bind test/python/stress/test_stress.cpp)
target_link_libraries(cpp_custom_bind PRIVATE autograd_core)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(NOT CLANG_FORMAT_EXE)
  message(FATAL_ERROR "clang-format not found!")
endif()

file(GLOB_RECURSE ALL_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_custom_target(
  format
  COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
  COMMENT "Running clang-format on source files..."
)