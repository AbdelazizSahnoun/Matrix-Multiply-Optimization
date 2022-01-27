#pragma once

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

class Matrix {
public:
  Matrix(size_t n);

  ~Matrix();

  size_t n() const;

  inline const float& operator()(size_t i, size_t j) const {
    assert(i < dim);
    assert(j < dim);
    return tab[j * dim + i];
  }

  inline float& operator()(size_t i, size_t j) {
    assert(i < dim);
    assert(j < dim);
    return tab[j * dim + i];
  }

  float* data() const;

  void fill_zero();
  void fill_random();
  void fill_linear();
  void dump(std::ofstream& os);

private:
  float* tab;
  size_t dim;
};
