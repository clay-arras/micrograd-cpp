#include "cuda_backend.hpp"
#include "autograd/cuda/basic_kern.cuh"
#include "autograd/cuda/binary_kern.cuh"
#include "autograd/cuda/matrix_kern.cuh"
#include "autograd/cuda/reduct_kern.cuh"
#include "autograd/cuda/unary_kern.cuh"
#include "autograd/engine/data_type.hpp"
#include "autograd/engine/device_type.hpp"
#include "autograd/engine/dispatch_type.hpp"
#include "autograd/engine/tensor_impl.hpp"

namespace autograd {

DataType CudaBackend::dtype_promotion_(DataType a_type, DataType b_type) {
  return static_cast<DataType>(
      std::max(static_cast<uint8_t>(a_type), static_cast<uint8_t>(b_type)));
}

TensorImpl CudaBackend::add(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  // NOTE: this is absolutely horrible
  DataType out_dtype = dtype_promotion_(a.type(), b.type());
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      DISPATCH_ALL_TYPES(out_dtype, [&] {
        using out_type = scalar_t;
        vecAdd<a_type_t, b_type_t, out_type>(
            a.size(), static_cast<const a_type_t*>(a.data()),
            static_cast<const b_type_t*>(b.data()),
            static_cast<out_type*>(c.data()));
      });
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::sub(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = dtype_promotion_(a.type(), b.type());
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      DISPATCH_ALL_TYPES(out_dtype, [&] {
        using out_type = scalar_t;
        vecSub<a_type_t, b_type_t, out_type>(
            a.size(), static_cast<const a_type_t*>(a.data()),
            static_cast<const b_type_t*>(b.data()),
            static_cast<out_type*>(c.data()));
      });
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::mul(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = dtype_promotion_(a.type(), b.type());
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      DISPATCH_ALL_TYPES(out_dtype, [&] {
        using out_type = scalar_t;
        vecMul<a_type_t, b_type_t, out_type>(
            a.size(), static_cast<const a_type_t*>(a.data()),
            static_cast<const b_type_t*>(b.data()),
            static_cast<out_type*>(c.data()));
      });
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::div(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = dtype_promotion_(a.type(), b.type());
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      DISPATCH_ALL_TYPES(out_dtype, [&] {
        using out_type = scalar_t;
        vecDiv<a_type_t, b_type_t, out_type>(
            a.size(), static_cast<const a_type_t*>(a.data()),
            static_cast<const b_type_t*>(b.data()),
            static_cast<out_type*>(c.data()));
      });
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::log(const TensorImpl& a) {
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    vecLog<scalar_t>(a.size(), static_cast<const scalar_t*>(a.data()),
                     static_cast<scalar_t*>(c.data()));
  });
  return TensorImpl(c, a.shape(), a.type());
}

TensorImpl CudaBackend::exp(const TensorImpl& a) {
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using scalar_t_ = scalar_t;
    vecExp<scalar_t_>(a.size(), static_cast<const scalar_t_*>(a.data()),
                      static_cast<scalar_t_*>(c.data()));
  });
  return TensorImpl(c, a.shape(), a.type());
}

TensorImpl CudaBackend::relu(const TensorImpl& a) {
  Storage c(a.size(), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using scalar_t_ = scalar_t;
    vecRelu<scalar_t_>(a.size(), static_cast<const scalar_t_*>(a.data()),
                       static_cast<scalar_t_*>(c.data()));
  });
  return TensorImpl(c, a.shape(), a.type());
}

TensorImpl CudaBackend::matmul(const TensorImpl& a, const TensorImpl& b) {
  assert(a.shape().size() == 2 && b.shape().size() == 2);
  assert(a.shape()[1] == b.shape()[0]);

  size_t m = a.shape()[0];
  size_t n = b.shape()[1];
  size_t k = a.shape()[1];

  DataType out_dtype = dtype_promotion_(a.type(), b.type());
  Storage c_storage(m * n, DeviceType::CUDA);

  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      DISPATCH_ALL_TYPES(out_dtype, [&] {
        using out_type_t = scalar_t;
        cudaMatMul<a_type_t, b_type_t, out_type_t>(
            static_cast<const a_type_t*>(a.data()),
            static_cast<const b_type_t*>(b.data()),
            static_cast<out_type_t*>(c_storage.data()), m, n, k);
      });
    });
  });
  return TensorImpl(c_storage, {m, n}, out_dtype);
}

TensorImpl CudaBackend::transpose(const TensorImpl& a) {
  assert(a.shape().size() == 2);
  size_t m = a.shape()[0];
  size_t n = a.shape()[1];

  DataType out_dtype = a.type();
  Storage c_storage(m * n, DeviceType::CUDA);

  DISPATCH_ALL_TYPES(a.type(), [&] {
    using scalar_t_ = scalar_t;
    cudaTranspose<scalar_t_>(static_cast<const scalar_t_*>(a.data()),
                             static_cast<scalar_t_*>(c_storage.data()), m, n);
  });

  return TensorImpl(c_storage, {n, m}, out_dtype);  // Shape is transposed
}

TensorImpl CudaBackend::equal(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecEqual<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::not_equal(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecNotEqual<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::greater_equal(const TensorImpl& a,
                                      const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecGreaterEqual<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::less_equal(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecLessEqual<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::greater(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecGreaterThan<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::less(const TensorImpl& a, const TensorImpl& b) {
  assert(a.size() == b.size());
  assert(a.shape() == b.shape());

  DataType out_dtype = DataType::Bool;
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  DISPATCH_ALL_TYPES(a.type(), [&] {
    using a_type_t = scalar_t;
    DISPATCH_ALL_TYPES(b.type(), [&] {
      using b_type_t = scalar_t;
      vecLessThan<a_type_t, b_type_t>(
          a.size(), static_cast<const a_type_t*>(a.data()),
          static_cast<const b_type_t*>(b.data()), static_cast<bool*>(c.data()));
    });
  });
  return TensorImpl(c, a.shape(), out_dtype);
}

TensorImpl CudaBackend::sum(const TensorImpl& a,
                            size_t axis) {  // NOTE: returning stub
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  return TensorImpl(c, a.shape(), a.type());
}

TensorImpl CudaBackend::max(const TensorImpl& a, size_t axis) {
  Storage c(a.size() * sizeof(bool), DeviceType::CUDA);
  return TensorImpl(c, a.shape(), a.type());
}

}  // namespace autograd