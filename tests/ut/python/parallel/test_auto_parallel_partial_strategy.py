# Copyright 2019 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np

import mindspore as ms
import mindspore.nn as nn
from mindspore import Tensor
from mindspore import context
from mindspore.common.api import _executor
from mindspore.ops import composite as C
from mindspore.ops import operations as P
from tests.ut.python.ops.test_math_ops import VirtualLoss


grad_all = C.GradOperation('get_all', get_all=True)


class NetWithLoss(nn.Cell):
    def __init__(self, network):
        super(NetWithLoss, self).__init__()
        self.loss = VirtualLoss()
        self.network = network

    def construct(self, x, y, z, w, b):
        predict = self.network(x, y, z, w, b)
        return self.loss(predict)


class GradWrap(nn.Cell):
    def __init__(self, network):
        super(GradWrap, self).__init__()
        self.network = network

    def construct(self, x, y, z, w, b):
        return grad_all(self.network)(x, y, z, w, b)

    # model_parallel test


def test_four_matmul_linear():
    class Net(nn.Cell):
        def __init__(self, strategy1):
            super().__init__()
            self.matmul1 = P.MatMul().set_strategy(strategy1)
            self.matmul2 = P.MatMul()
            self.matmul3 = P.MatMul()
            self.matmul4 = P.MatMul()

        def construct(self, x, y, z, w, b):
            out = self.matmul1(x, y)
            out = self.matmul2(out, z)
            out = self.matmul3(out, w)
            out = self.matmul4(out, b)
            return out

    size = 64
    context.set_auto_parallel_context(device_num=size, global_rank=0)
    strategy1 = ((2, 4), (4, 8))
    x = Tensor(np.ones([128, 32]), dtype=ms.float32)
    y = Tensor(np.ones([32, 64]), dtype=ms.float32)
    z = Tensor(np.ones([64, 32]), dtype=ms.float32)
    w = Tensor(np.ones([32, 32]), dtype=ms.float32)
    b = Tensor(np.ones([32, 256]), dtype=ms.float32)

    net = GradWrap(NetWithLoss(Net(strategy1)))
    context.set_auto_parallel_context(parallel_mode="auto_parallel")
    net.set_auto_parallel()
    _executor.compile(net, x, y, z, w, b)
