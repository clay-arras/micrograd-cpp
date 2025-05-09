#pragma once

#include <cstddef>
#include "autograd/engine/data_ptr.hpp"
#include "autograd/engine/device_type.hpp"
#include "autograd/engine/dispatch_stub.hpp"

namespace autograd {

using resize_fn = void (*)(DataPtr, size_t, size_t);
DECLARE_DISPATCH(resize_fn, resize_stub);

void resize_cpu(DataPtr dptr, size_t old_byte_size, size_t new_byte_size);
void resize_cuda(DataPtr dptr, size_t old_byte_size, size_t new_byte_size);

REGISTER_DISPATCH(resize_stub, DeviceType::CPU, resize_cpu);
REGISTER_DISPATCH(resize_stub, DeviceType::CUDA, resize_cuda);

}  // namespace autograd
