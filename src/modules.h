#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "gemm.h"

typedef void (*gemm_func)(const Matrix&, const Matrix&, Matrix&);

struct gemm_klass {
  std::string label;
  gemm_func func;
};

extern const std::vector<struct gemm_klass> algo;
