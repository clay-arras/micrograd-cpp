#include <Eigen/Core>
#include <cmath>
#include "autograd/engine/wrapper_engine.h"

/**
 * @brief Specialization of NumTraits for SharedValue type.
 *
 * This struct provides type traits for the SharedValue type, allowing it to be
 * used seamlessly with Eigen's matrix and array types. It inherits from NumTraits<float>
 * and defines properties specific to SharedValue.
 */
namespace Eigen {
template <>
struct NumTraits<SharedValue> : NumTraits<float> {
  using Real = SharedValue;  ///< The real type corresponding to SharedValue.
  using NonInteger =
      SharedValue;  ///< The non-integer type corresponding to SharedValue.
  using Nested =
      SharedValue;  ///< The nested type corresponding to SharedValue.

  enum {
    IsComplex = 0,  ///< Indicates that SharedValue is not a complex type.
    IsInteger = 0,  ///< Indicates that SharedValue is not an integer type.
    IsSigned = 1,   ///< Indicates that SharedValue is a signed type.
    RequireInitialization =
        1,         ///< Indicates that SharedValue requires initialization.
    ReadCost = 1,  ///< The cost of reading a SharedValue.
    AddCost = 1,   ///< The cost of adding two SharedValues.
    MulCost = 1    ///< The cost of multiplying two SharedValues.
  };

  /**
   * @brief Returns the machine epsilon for SharedValue.
   *
   * This value represents the smallest difference between two representable numbers
   * that can be distinguished by SharedValue.
   * @return A SharedValue representing the machine epsilon.
   */
  static Real epsilon() { return SharedValue(1e-10); }

  /**
   * @brief Returns a dummy precision value for SharedValue.
   *
   * This value can be used as a placeholder for precision-related calculations.
   * @return A SharedValue representing the dummy precision.
   */
  static Real dummy_precision() { return SharedValue(1e-10); }

  /**
   * @brief Returns the identity value for SharedValue.
   *
   * This value represents the multiplicative identity for SharedValue.
   * @return A SharedValue representing the identity value.
   */
  static Real Identity() { return SharedValue(1.0); }
};
}  // namespace Eigen