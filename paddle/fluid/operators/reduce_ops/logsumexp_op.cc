// Copyright (c) 2020 PaddlePaddle Authors. All Rights Reserved.
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

#include <algorithm>
#include <string>
#include <vector>

#include "paddle/fluid/framework/infershape_utils.h"
#include "paddle/fluid/framework/op_registry.h"
#include "paddle/phi/core/infermeta_utils.h"
#include "paddle/phi/infermeta/unary.h"

namespace paddle {
namespace operators {

class LogsumexpOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;
};

class LogsumexpOpMaker : public framework::OpProtoAndCheckerMaker {
 public:
  void Make() override {
    AddInput("X",
             "(Tensor) The input tensor. Tensors with rank at most 4 are "
             "supported.");
    AddOutput("Out", "(Tensor) The result tensor.");
    AddAttr<std::vector<int>>(
        "axis",
        "(list<int>, default {0}) The dimensions to reduce. "
        "Must be in the range [-rank(input), rank(input)). "
        "If `axis[i] < 0`, the axis[i] to reduce is `rank + axis[i]`. "
        "Note that reducing on the first dim will make the LoD info lost.")
        .SetDefault({0});
    AddAttr<bool>("keepdim",
                  "(bool, default false) "
                  "If true, retain the reduced dimension with length 1.")
        .SetDefault(false);
    AddAttr<bool>("reduce_all",
                  "(bool, default false) "
                  "If true, output a scalar reduced along all dimensions.")
        .SetDefault(false);
    AddComment(string::Sprintf(R"DOC(
logsumexp Operator.

This operator computes the logsumexp of input tensor along the given axis.
The result tensor has 1 fewer dimension than the input unless keep_dim is true.
If reduce_all is true, just reduce along all dimensions and output a scalar.

)DOC"));
  }
};

class LogsumexpGradOp : public framework::OperatorWithKernel {
 public:
  using framework::OperatorWithKernel::OperatorWithKernel;

  void InferShape(framework::InferShapeContext* ctx) const override {
    OP_INOUT_CHECK(ctx->HasInput("X"), "Input", "X", "logsumexp");
    OP_INOUT_CHECK(ctx->HasInput("Out"), "Input", "Out", "logsumexp");
    OP_INOUT_CHECK(ctx->HasInput(framework::GradVarName("Out")),
                   "Input",
                   "Out@GRAD",
                   "logsumexp");
    ctx->SetOutputDim(framework::GradVarName("X"), ctx->GetInputDim("X"));
  }
};

template <typename T>
class LogsumexpGradOpMaker : public framework::SingleGradOpMaker<T> {
 public:
  using framework::SingleGradOpMaker<T>::SingleGradOpMaker;

  void Apply(GradOpPtr<T> op) const override {
    op->SetType("logsumexp_grad");
    op->SetInput("X", this->Input("X"));
    op->SetInput("Out", this->Output("Out"));
    op->SetInput(framework::GradVarName("Out"), this->OutputGrad("Out"));
    op->SetAttrMap(this->Attrs());
    op->SetOutput(framework::GradVarName("X"), this->InputGrad("X"));
  }
};

}  // namespace operators
}  // namespace paddle

namespace ops = paddle::operators;
DECLARE_INFER_SHAPE_FUNCTOR(logsumexp,
                            LogsumexpInferShapeFunctor,
                            PD_INFER_META(phi::LogsumexpInferMeta));
REGISTER_OPERATOR(logsumexp,
                  ops::LogsumexpOp,
                  ops::LogsumexpOpMaker,
                  ops::LogsumexpGradOpMaker<paddle::framework::OpDesc>,
                  ops::LogsumexpGradOpMaker<paddle::imperative::OpBase>,
                  LogsumexpInferShapeFunctor);
REGISTER_OPERATOR(logsumexp_grad, ops::LogsumexpGradOp);
