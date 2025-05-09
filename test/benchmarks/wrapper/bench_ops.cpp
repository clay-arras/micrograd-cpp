/* 04/09/25
----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
BM_OperationsSharedValue      21551953 ns     21531577 ns           32
BM_OperationsSharedPtrValue   20713172 ns     20699420 ns           34
BM_OperationsVariable         19521069 ns     19509833 ns           32
BM_OperationsDouble              82369 ns        82341 ns         8579
BM_OperationsDoubleWrapper      174572 ns       174547 ns         3864
*/
#include <benchmark/benchmark.h>
#include "autograd/engine/variable.h"
#include "wrapper_engine.h"

namespace {

// Generate a random float value between -1 and 1
float generateRandom() {
  return (2.0F * (static_cast<float>(rand()) / RAND_MAX) - 1.0F) / 1000;
}

void BM_OperationsSharedValue(benchmark::State& state) {
  const int iterations = 10000;
  std::vector<SharedValue> values1(iterations);
  std::vector<SharedValue> values2(iterations);

  for (int i = 0; i < iterations; ++i) {
    values1[i] = SharedValue(generateRandom());
    values2[i] = SharedValue(generateRandom());
  }

  for (auto _ : state) {
    for (int i = 0; i < iterations; ++i) {
      SharedValue add = values1[i] + values2[i];
      SharedValue sub = values1[i] - values2[i];
      SharedValue mul = values1[i] * values2[i];
      SharedValue div = values1[i] / values2[i];
      benchmark::DoNotOptimize(add);
      benchmark::DoNotOptimize(sub);
      benchmark::DoNotOptimize(mul);
      benchmark::DoNotOptimize(div);
    }
  }
}

void BM_OperationsVariable(benchmark::State& state) {
  const int iterations = 10000;
  std::vector<Variable> values1(iterations);
  std::vector<Variable> values2(iterations);

  for (int i = 0; i < iterations; ++i) {
    values1[i] = Variable(generateRandom());
    values2[i] = Variable(generateRandom());
  }

  for (auto _ : state) {
    for (int i = 0; i < iterations; ++i) {
      Variable add = values1[i] + values2[i];
      Variable sub = values1[i] - values2[i];
      Variable mul = values1[i] * values2[i];
      Variable div = values1[i] / values2[i];
      benchmark::DoNotOptimize(add);
      benchmark::DoNotOptimize(sub);
      benchmark::DoNotOptimize(mul);
      benchmark::DoNotOptimize(div);
    }
  }
}

void BM_OperationsDouble(benchmark::State& state) {
  const int iterations = 10000;
  std::vector<double> values1(iterations);
  std::vector<double> values2(iterations);

  for (int i = 0; i < iterations; ++i) {
    values1[i] = generateRandom();
    values2[i] = generateRandom();
  }

  for (auto _ : state) {
    for (int i = 0; i < iterations; ++i) {
      double add = values1[i] + values2[i];
      double sub = values1[i] - values2[i];
      double mul = values1[i] * values2[i];
      double div = values1[i] / values2[i];
      benchmark::DoNotOptimize(add);
      benchmark::DoNotOptimize(sub);
      benchmark::DoNotOptimize(mul);
      benchmark::DoNotOptimize(div);
    }
  }
}
struct DoubleWrapper {
  double value;

  explicit DoubleWrapper(double val = 0.0) : value(val) {}

  DoubleWrapper operator+(const DoubleWrapper& other) const {
    return DoubleWrapper(value + other.value);
  }

  DoubleWrapper operator-(const DoubleWrapper& other) const {
    return DoubleWrapper(value - other.value);
  }

  DoubleWrapper operator*(const DoubleWrapper& other) const {
    return DoubleWrapper(value * other.value);
  }

  DoubleWrapper operator/(const DoubleWrapper& other) const {
    return DoubleWrapper(value / other.value);
  }
};

void BM_OperationsDoubleWrapper(benchmark::State& state) {
  const int iterations = 10000;
  std::vector<DoubleWrapper> values1(iterations);
  std::vector<DoubleWrapper> values2(iterations);

  for (int i = 0; i < iterations; ++i) {
    values1[i] = DoubleWrapper(generateRandom());
    values2[i] = DoubleWrapper(generateRandom());
  }

  for (auto _ : state) {
    for (int i = 0; i < iterations; ++i) {
      DoubleWrapper add = values1[i] + values2[i];
      DoubleWrapper sub = values1[i] - values2[i];
      DoubleWrapper mul = values1[i] * values2[i];
      DoubleWrapper div = values1[i] / values2[i];
      benchmark::DoNotOptimize(add);
      benchmark::DoNotOptimize(sub);
      benchmark::DoNotOptimize(mul);
      benchmark::DoNotOptimize(div);
    }
  }
}

void BM_OperationsSharedPtrValue(benchmark::State& state) {
  const int iterations = 10000;
  std::vector<std::shared_ptr<Value>> values1(iterations);
  std::vector<std::shared_ptr<Value>> values2(iterations);

  for (int i = 0; i < iterations; ++i) {
    values1[i] = std::make_shared<Value>(generateRandom());
    values2[i] = std::make_shared<Value>(generateRandom());
  }

  for (auto _ : state) {
    for (int i = 0; i < iterations; ++i) {
      auto add = values1[i] + values2[i];
      auto sub = values1[i] - values2[i];
      auto mul = values1[i] * values2[i];
      auto div = values1[i] / values2[i];
      benchmark::DoNotOptimize(add);
      benchmark::DoNotOptimize(sub);
      benchmark::DoNotOptimize(mul);
      benchmark::DoNotOptimize(div);
    }
  }
};

BENCHMARK(BM_OperationsSharedValue);
BENCHMARK(BM_OperationsSharedPtrValue);
BENCHMARK(BM_OperationsVariable);
BENCHMARK(BM_OperationsDouble);
BENCHMARK(BM_OperationsDoubleWrapper);

}  // namespace
BENCHMARK_MAIN();