#include "empty.cuh"

namespace autograd {

DEFINE_DISPATCH(empty_stub);

DataPtr empty_cpu(size_t byte_size) {
  void* ptr_ = ::operator new(byte_size);
  return DataPtr(ptr_, std::make_shared<std::function<void(void*)>>(
                           [](void* ptr) { ::operator delete(ptr); }));
}
DataPtr empty_cuda(size_t byte_size) {
  void* ptr_ = nullptr;
  cudaMalloc(&ptr_, byte_size);
  return DataPtr(ptr_, std::make_shared<std::function<void(void*)>>(
                           [](void* ptr) { cudaFree(ptr); }));
}

}  // namespace autograd