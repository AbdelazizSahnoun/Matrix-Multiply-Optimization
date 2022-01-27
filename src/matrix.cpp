#include "matrix.h"

#include <fstream>
#include <functional>
#include <iomanip>
#include <random>

Matrix::Matrix(size_t n) : dim(n){
  tab = static_cast<float*>(aligned_alloc(64, n * n * sizeof(float)));
  fill_zero();
}

Matrix::~Matrix() { free(tab); }

size_t Matrix::n() const { return dim; }

float* Matrix::data() const { return tab; }

void Matrix::fill_zero() { std::fill(tab, tab + dim * dim, 0.0f); }

void Matrix::fill_random() {
  std::random_device device;
  std::mt19937 generator(device());
  std::uniform_real_distribution<float> distrib(0, 1);

  for (size_t i = 0; i < n(); i++) {
    for (size_t j = 0; j < n(); j++) {
      operator()(i, j) = distrib(generator);
    }
  }
}

void Matrix::fill_linear() {
  for (size_t j = 0; j < n(); j++) {
    for (size_t i = 0; i < n(); i++) {
      operator()(i, j) = (j * n() + i)*0.001f;
    }
  }
}

void Matrix::dump(std::ofstream& os) {
  for (size_t i = 0; i < dim; ++i) {
    for (size_t j = 0; j < dim; j++) {
      os << std::setw(6) << operator()(i, j) << " ";
    }
    os << "\n";
  }
}
