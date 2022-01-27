#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "matrix.h"
#include "modules.h"

int matrix_check(const Matrix& a, const Matrix& b, std::ofstream& log) {
  int ret = 0;
  for (size_t j = 0; j < a.n(); j++) {
    for (size_t i = 0; i < a.n(); i++) {
      float diff = std::abs(a(i, j) - b(i, j));
      if (diff > 1e-3f) {
        log << "Error at index (" << i << "," << j << ") " << a(i, j) << " != " << b(i, j)
            << " diff=" << diff << "\n";
        ret = 1;
      }
    }
  }
  return ret;
}

int test_one(const gemm_klass& klass, size_t n, std::string& name) {
  Matrix A(n);
  Matrix B(n);
  Matrix C(n);
  Matrix ref(n);
  Matrix err(n);

  A.fill_linear();
  B.fill_linear();

  // On prend gemm_basic() comme référence, car on sait que cet algorithme
  // fonctionne dans tous les cas.
  gemm_basic(A, B, ref);

  C.fill_zero();
  klass.func(A, B, C);

  // enregistre resultat
  std::ofstream out(name + ".log");
  out << "Matrice A:\n";
  A.dump(out);

  out << "Matrice B:\n";
  B.dump(out);

  out << "Matrice C:\n";
  C.dump(out);

  out << "Matrice Reference:\n";
  ref.dump(out);

  for (size_t j = 0; j < A.n(); j++) {
    for (size_t i = 0; i < A.n(); i++) {
      err(i, j) = ref(i, j) - C(i, j);
    }
  }

  out << "Matrice Ref - C:\n";
  err.dump(out);

  return matrix_check(ref, C, out);
}

int main(int argc, char** argv) {
  int ret = 0;
  std::vector<size_t> sizes = {4, 8, 12, 16, 32, 36, 64, 68};

  bool filter = false;
  std::string filter_test_name;
  if (argc > 1) {
    filter_test_name = argv[1];
    filter = true;
  }

  for (const gemm_klass& klass : algo) {
    for (const size_t n : sizes) {
      std::string test_name = klass.label + "_" + std::to_string(n);
      if (!filter || test_name == filter_test_name) {
        int res = test_one(klass, n, test_name);
        std::string status = res == 0 ? "PASS" : "FAIL";
        std::cout << "TEST " << test_name << " : " << status << std::endl;
        ret |= res;
      }
    }
  }

  std::printf("done\n");
  return ret;
}
