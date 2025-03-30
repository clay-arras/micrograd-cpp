#include "fast_layer.h"

FastLayer::FastLayer(int nin, int nout) { 
    weights.resize(nout, nin);
    bias.resize(nout, 1);

    std::random_device seed;
    std::mt19937 gen{seed()};
    std::uniform_real_distribution<> dist{-1.0, 1.0};
    auto initValue = [&dist, &gen](const std::shared_ptr<Value> &a) {
        return std::make_shared<Value>(dist(gen));
    };
    weights = weights.unaryExpr(initValue);
    bias = bias.unaryExpr(initValue);
}

/**
 * @brief Forward pass through the FastLayer.
 *
 * This operator takes an input vector and applies the layer's weights and bias,
 * followed by an activation function.
 *
 * @param x Input vector of shape (nin x 1), where nin is the number of input features.
 * @param activ Activation function to be applied to the output after the linear transformation.
 * 
 * @return Eigen::Matrix<std::shared_ptr<Value>, Eigen::Dynamic, 1> The output vector after applying
 *         the weights, bias, and activation function, of shape (nout x 1), where nout is the number
 *         of output features.
 */
Eigen::Matrix<std::shared_ptr<Value>, Eigen::Dynamic, 1>
FastLayer::operator()(Eigen::Matrix<std::shared_ptr<Value>, Eigen::Dynamic, 1> x, std::function<std::shared_ptr<Value>(std::shared_ptr<Value>)> activ) {
    Eigen::Matrix<std::shared_ptr<Value>, Eigen::Dynamic, 1> a = weights * x;

    a = a + bias;
    Eigen::Matrix<std::shared_ptr<Value>, Eigen::Dynamic, 1> z = a.unaryExpr(activ);
    return z;
}