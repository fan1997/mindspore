/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_FP32_CONV_H_
#define MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_FP32_CONV_H_

#ifdef ENABLE_NEON
#include <arm_neon.h>
#endif
#include "nnacl/pack.h"
#include "nnacl/op_base.h"
#include "nnacl/common_func.h"
#include "nnacl/conv_parameter.h"
#include "nnacl/fp32/strassen_matmul.h"
#include "nnacl/winograd_utils.h"

using TmpBufferAddress = float *;
typedef void (*GEMM_FUNC_FP32)(float *output, const float *input, const float *weight, const float *bias, size_t step,
                               size_t ic4, size_t output_channel, size_t offset, size_t mode, size_t writeC4,
                               size_t relu, size_t relu6);

// fp32 convolution common (im2col+gemm)
void ConvFp32(float *input_data, float *packed_input, float *packed_weight, const float *bias_data,
              float *tmp_out_block, float *output_data, int task_id, ConvParameter *conv_param,
              GEMM_FUNC_FP32 gemm_func);

// fp32 conv1x1 strassen matmul
int Conv1x1Fp32(const float *input_data, const float *weight_data, float *output_data, float *tmp_ptr,
                StrassenMatMulParameter matmul_param);

// fp32 convolution winograd
void ConvWinogardFp32(float *input_data, float *trans_weight, const float *bias_data, float *output_data,
                      TmpBufferAddress *buffer_list, int task_id, ConvParameter *conv_param,
                      InputTransformUnitFunc input_trans_func, OutputTransformUnitFunc output_trans_func,
                      GEMM_FUNC_FP32 gemm_func);

void UnPackWinogradOutput(const float *src, float *dst, int batch, int height, int width, int channel, int output_unit);

// fp32 conv3x3
void Conv3x3Fp32(float *input_data, float *transed_weight, const float *bias_data, float *output_data,
                 TmpBufferAddress *buffer_list, int task_id, ConvParameter *conv_param, GEMM_FUNC_FP32 gemm_func);

#endif  // MINDSPORE_LITE_SRC_RUNTIME_KERNEL_ARM_NNACL_FP32_CONV_H_
