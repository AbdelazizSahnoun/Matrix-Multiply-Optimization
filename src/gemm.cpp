
#include "gemm.h"
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <immintrin.h>
#include <xmmintrin.h>

#define BLOCK_SIZE 32

// imprimer un registre __m128 sous forme de 4 nombres simple precision
/*
std::ostream& operator<<(std::ostream& os, __m128 const& v) {
  return os << "[" << v[3] << "|" << v[2] << "|" << v[1] << "|" << v[0] << "]";
}
*/

void gemm_basic(const Matrix& A, const Matrix& B, Matrix& C) {
  const size_t n = C.n();
  assert(A.n() == n && B.n() == n);

  for (size_t j = 0; j < n; ++j) {
    for (size_t i = 0; i < n; ++i) {
      float cij = C(i, j);
      for (size_t k = 0; k < n; ++k) {
        cij += A(i, k) * B(k, j);
      }
      C(i, j) = cij;
    }
  }
}

void gemm_simd(const Matrix& A, const Matrix& B, Matrix& C) {
  const size_t n = C.n();
  assert(A.n() == n && B.n() == n);
  // À COMPLÉTER

  for(size_t i_A=0;i_A<A.n();i_A+=4) {
    for (size_t j_B=0; j_B < B.n(); j_B++) {

      __m128 C_value = _mm_setzero_ps();

      for (size_t i_B = 0; i_B < B.n(); i_B++) {
        const __m128 B_value = _mm_set1_ps(B(i_B, j_B));
        const __m128 A_values
            = _mm_setr_ps(A(i_A, i_B), A(i_A + 1, i_B), A(i_A + 2, i_B), A(i_A + 3, i_B));
        C_value = _mm_add_ps(C_value, _mm_mul_ps(B_value, A_values));
      }


      for (int i = 0; i < 4; i++) {
        C(i + i_A, j_B) = C_value[i];
      }

    }

  }




}

void gemm_blocked(const Matrix& A, const Matrix& B, Matrix& C) {
  const size_t n = C.n();
  assert(A.n() == n && B.n() == n);



  const size_t block_size=BLOCK_SIZE;

  size_t divison=A.n()/block_size;
  size_t modulo=A.n()%block_size;

  //cas 36,68,etc ceux qui ne fit pas dans 32 éléments géré par openMP
  if(modulo!=0){
    gemm_omp(A,B,C);
  } else {
    for (int i_C = 0; i_C < divison * block_size; i_C += block_size) {
      for (int j_C = 0; j_C < divison * block_size; j_C += block_size) {
        auto a_block = new float[block_size][block_size];
        for (int i_A = 0; i_A <  block_size; i_A++) {
          for (int j_A = 0; j_A < block_size; j_A++) {
            a_block[i_A][j_A] = (A(i_A + i_C, j_A + j_C));
          }
        }

        for (int i_B = 0; i_B < divison * block_size; i_B += block_size) {
          auto b_block = new float[block_size][block_size];
          for (int ii_b = 0; ii_b < block_size; ii_b++) {
            for (int jj_b = 0; jj_b < block_size; jj_b++) {
              b_block[ii_b][jj_b] = (B(ii_b + j_C, jj_b + i_B));

            }
          }

          for (int k = 0; k < block_size; k++) {
            for (int kk = 0; kk < block_size; kk++) {
              float accum = C(k + i_C, kk + i_B);
              for (int z = 0; z < block_size; z++) {
                accum += a_block[k][z] * b_block[z][kk];
              }
              C(k + i_C, kk + i_B) = accum;
            }
          }
          delete[] b_block;
        }
        delete[] a_block;
      }
    }
  }





}

void gemm_omp(const Matrix& A, const Matrix& B, Matrix& C) {
  const size_t n = C.n();
  assert(A.n() == n && B.n() == n);

  int j,i,k;
  float cij;

#pragma omp parallel for private(i,j,k) reduction(+:cij)
  for (j = 0; j < n; ++j) {
    for (i = 0; i < n; ++i) {
      cij = C(i, j);
      for (k = 0; k < n; ++k) {
        cij += A(i, k) * B(k, j);
      }
      C(i, j) = cij;
    }
  }
}

// combined techniques
void gemm_combo(const Matrix& A, const Matrix& B, Matrix& C) {
  const size_t n = C.n();
  assert(A.n() == n && B.n() == n);

  const size_t block_size=BLOCK_SIZE;

  size_t divison=A.n()/block_size;
  size_t modulo=A.n()%block_size;

  size_t i_C,j_C,i_A,j_A,i_B,ii_b,jj_b,k,kk,z=0;
  float accuml;

  if(modulo!=0){
    gemm_omp(A,B,C);
  } else {
#pragma omp parallel for private(i_C,j_C,i_A,j_A,i_B,ii_b,jj_b,k,kk,z) reduction(+:accuml)
    for ( i_C = 0; i_C < divison * block_size; i_C += block_size) {
      for ( j_C = 0; j_C < divison * block_size; j_C += block_size) {
        auto a_block = new float[block_size][block_size];
        for ( i_A = 0; i_A < block_size; i_A++) {
          for ( j_A = 0; j_A < block_size; j_A+=4) {
            const __m128 A_values
                = _mm_setr_ps(A(i_A + i_C, (j_A+0) + j_C),
                              A(i_A + i_C, (j_A+1) + j_C),
                              A(i_A + i_C, (j_A+2) + j_C),
                              A(i_A + i_C, (j_A+3) + j_C));
            _mm_store_ps(&a_block[i_A][j_A],A_values);
          }
        }

        for ( i_B = 0; i_B < divison * block_size; i_B += block_size) {
          auto b_block = new float[block_size][block_size];
          for ( ii_b = 0; ii_b < block_size; ii_b++) {
            for ( jj_b = 0; jj_b < block_size; jj_b+=4) {
              const __m128 B_values
                  = _mm_setr_ps(B(ii_b + j_C, (jj_b+0) + i_B),
                                B(ii_b + j_C, (jj_b+1) + i_B),
                                B(ii_b + j_C, (jj_b+2) + i_B),
                                B(ii_b + j_C, (jj_b+3) + i_B));
              _mm_store_ps(&b_block[ii_b][jj_b],B_values);

            }
          }

          for ( k = 0; k < block_size; k++) {
            for ( kk = 0; kk < block_size; kk++) {
              accuml = C(k + i_C, kk + i_B);
              for ( z = 0; z < block_size; z+=4) {
                __m128 a_values=_mm_setr_ps(a_block[k][z],a_block[k][z+1],a_block[k][z+2],a_block[k][z+3]);
                __m128 b_values=_mm_setr_ps(b_block[z][kk],b_block[z+1][kk],b_block[z+2][kk],b_block[z+3][kk]);

                __m128 result = _mm_mul_ps(a_values,b_values);

                accuml += result[0]+result[1]+result[2]+result[3];

              }
              C(k + i_C, kk + i_B) = accuml;
            }
          }
          delete[] b_block;
        }
        delete[] a_block;
      }
    }
  }




}

#ifdef HAS_EIGEN
#  include <Eigen/Dense>

void gemm_eigen(const Matrix& A, const Matrix& B, Matrix& C) {
  Eigen::Map<Eigen::MatrixXf> mat_a(A.data(), A.n(), A.n());
  Eigen::Map<Eigen::MatrixXf> mat_b(B.data(), B.n(), B.n());
  Eigen::Map<Eigen::MatrixXf> mat_c(C.data(), C.n(), C.n());

  mat_c = mat_a * mat_b;
}
#endif
