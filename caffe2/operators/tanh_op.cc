#include <cmath>

#include "caffe2/operators/elementwise_op.h"

namespace caffe2 {

struct TanhCPUFunctor {
  template <typename T>
  inline void operator()(const int n, const T* x,
                         T* y, CPUContext* device_context) {
    for (int i = 0; i < n; ++i) {
      y[i] = tanh(x[i]);
    }
  }
};

struct TanhGradientCPUFunctor {
  template <typename T>
  inline void operator()(const int n, const T* y, const T* dy,
                         T* dx, CPUContext* device_context) {
    for (int i = 0; i < n; ++i) {
      dx[i] = dy[i] * (1 - y[i] * y[i]);
    }
  }
};

namespace {
REGISTER_CPU_OPERATOR(
    Tanh, UnaryElementwiseOp<TensorTypes<float>, CPUContext, TanhCPUFunctor>);
REGISTER_CPU_OPERATOR(
    TanhGradient, BinaryElementwiseOp<TensorTypes<float>, CPUContext,
                                     TanhGradientCPUFunctor>);

OPERATOR_SCHEMA(Tanh)
  .NumInputs(1)
  .NumOutputs(1)
  .AllowInplace({{0, 0}})
  .SetDoc(R"DOC(
Calculates the hyperbolic tangent of the given input tensor element-wise. This
operation can be done in an in-place fashion too, by providing the same input
and output blobs.
)DOC")
  .Input(0, "input", "1-D input tensor")
  .Output(1, "output", "The hyperbolic tangent values of the input tensor "
          "computed element-wise");

OPERATOR_SCHEMA(TanhGradient).NumInputs(2).NumOutputs(1).AllowInplace({{1, 0}});

class GetTanhGradient : public GradientMakerBase {
  using GradientMakerBase::GradientMakerBase;
  vector<OperatorDef> GetGradientDefs() override {
    return SingleGradientDef(
        "TanhGradient", "",
        std::vector<string>{O(0), GO(0)},
        std::vector<string>{GI(0)});
  }
};
REGISTER_GRADIENT(Tanh, GetTanhGradient);
}  // namespace
}  // namespace caffe2
