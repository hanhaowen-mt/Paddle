// Copyright (c) 2024 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "paddle/phi/core/dense_tensor.h"

namespace phi {
template <typename T, typename Context>
void DependKernel(const Context& dev_ctx,
                  const DenseTensor& x,
                  const std::vector<const DenseTensor*>& dep,
                  DenseTensor* out) {
  auto x_name = &x;
  auto out_name = out;
  PADDLE_ENFORCE_EQ(x_name,
                    out_name,
                    phi::errors::PreconditionNotMet(
                        "Input(X) and Output(Out) variable should be the "
                        "same, but got Input is %s and Output is %s.",
                        x_name,
                        out_name));
}
}  // namespace phi
