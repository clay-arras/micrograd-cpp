# adapted and extended from Karpathy's micrograd
from learning.micrograd_engine import Value
from abc import ABC, abstractmethod
import random


class Module(ABC):
    def zero_grad(self):
        for p in self.parameters():
            p.grad = 0

    @abstractmethod
    def parameters(self):
        pass


class Neuron:
    def __init__(self, nin):
        self.weights = [Value(random.uniform(-1.0, 1.0)) for i in range(nin)]
        self.bias = Value(random.uniform(-1.0, 1.0))

    def __call__(self, x):  # forward pass
        _sum = self.bias
        for xi, wi in zip(x, self.weights):
            _sum = _sum + (xi * wi)
        return _sum

    def parameters(self):
        return self.weights + [self.bias]


class Layer:
    def __init__(self, nin, nout):
        self.neurons = [Neuron(nin) for i in range(nout)]

    def __call__(self, x, relu=True):  # x is nin
        out = [neuron(x) for neuron in self.neurons]
        if relu:
            out = [i.relu() for i in out]
        return out[0] if len(out) == 1 else out

    def parameters(self):
        params = []
        for neuron in self.neurons:
            ps = neuron.parameters()
            params.extend(ps)
        return params
        # return [p for neuron in self.neurons for p in neuron.parameters()]


class MLP:
    def __init__(self, nin, nouts):
        self.layers = []
        prev = nin
        for i in nouts:
            self.layers.append(Layer(prev, i))
            prev = i

    def __call__(self, x):  # forward pass
        y = x
        for i in self.layers:
            y = i(y)
        return y

    def parameters(self):
        params = []
        for layer in self.layers:
            ps = layer.parameters()
            params.extend(ps)
        return params
