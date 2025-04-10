cmake_minimum_required(VERSION 3.10)
project(autograd_cpp)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")

find_package(Eigen3 REQUIRED)
find_package(benchmark REQUIRED)

add_library(autograd_core # next step: split into different bench envs
        # test/benchmarks/wrapper/engine.cpp
        # test/benchmarks/wrapper/grad.cpp
        # test/benchmarks/wrapper/wrapper_engine.cpp
        # test/benchmarks/mem_pool/value_pool.cpp
    autograd/engine/variable.cpp
    autograd/engine/functions/basic_ops.cpp
    autograd/engine/functions/unary_ops.cpp
        # test/benchmarks/mem_pool/variable_mem.cpp
    autograd/engine/function.cpp
    autograd/engine/tensor.cpp
#    test/benchmarks/nets/nn.cpp
#    test/benchmarks/nets/fast_layer.cpp
    autograd/util/csv_reader.cpp
        # test/benchmarks/mem_pool/variable_pool.cpp
        # test/benchmarks/mem_pool/variable_pool.h
)

target_include_directories(autograd_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${EIGEN3_INCLUDE_DIRS}
)

#add_executable(test_engine test/cpp/test_engine.cpp)
#target_link_libraries(test_engine autograd_core)
#
#add_executable(test_nn test/cpp/test_nn.cpp)
#target_link_libraries(test_nn autograd_core)

#add_executable(mnist autograd/mnist/mnist.cpp)
#target_link_libraries(mnist autograd_core)
#
#add_executable(fast_mnist autograd/mnist/fast_mnist.cpp)
#target_link_libraries(fast_mnist autograd_core)

# add_executable(test_mnist test/cpp/test_mnist.cpp)
# target_link_libraries(test_mnist autograd_core)

# add_executable(test_fast_mnist test/cpp/test_fast_mnist.cpp)
# target_link_libraries(test_fast_mnist autograd_core)

# add_executable(test_mem_pool test/cpp/test_mem_pool.cpp)
# target_link_libraries(test_mem_pool autograd_core)

#add_executable(bench_nn test/benchmarks/bench_nn.cpp)
#target_link_libraries(bench_nn autograd_core benchmark::benchmark)
#
# add_executable(bench_nets test/benchmarks/nets/bench_nets.cpp)
# target_link_libraries(bench_nets autograd_core benchmark::benchmark)

#  add_executable(bench_mat test/benchmarks/wrapper/bench_mat.cpp)
#  target_link_libraries(bench_mat autograd_core benchmark::benchmark)

#  add_executable(bench_ops test/benchmarks/wrapper/bench_ops.cpp)
#  target_link_libraries(bench_ops autograd_core benchmark::benchmark)

# add_executable(bench_mem_pool test/benchmarks/mem_pool/bench_mem_pool.cpp)
# target_link_libraries(bench_mem_pool autograd_core benchmark::benchmark)

add_executable(test_playground test/cpp/test_playground.cpp)
target_link_libraries(test_playground autograd_core benchmark::benchmark)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if (NOT CLANG_FORMAT_EXE)
  message(FATAL_ERROR "clang-format not found!")
endif()

file(GLOB_RECURSE ALL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on source files..."
)
