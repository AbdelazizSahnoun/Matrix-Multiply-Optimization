#pragma once

#include "matrix.h"

/**
 * @brief gemm_basic: implémentation de référence du produit matriciel, implémentation simple avec 3
 * boucles imbriquées.
 */
void gemm_basic(const Matrix& A, const Matrix& B, Matrix& C);

/**
 * @brief gemm_simd: produit matriciel avec instructions vectorielles
 */
void gemm_simd(const Matrix& A, const Matrix& B, Matrix& C);

/**
 * @brief gemm_blocked: produit matriciel avec intervalles 2D
 */
void gemm_blocked(const Matrix& A, const Matrix& B, Matrix& C);

/**
 * @brief gemm_omp: produit matriciel avec parallélisation OpenMP
 */
void gemm_omp(const Matrix& A, const Matrix& B, Matrix& C);

/**
 * @brief gemm_combo: produit matriciel combinant simd, intervalled 2D et OpenMP
 */
void gemm_combo(const Matrix& A, const Matrix& B, Matrix& C);

#if HAS_EIGEN
/**
 * @brief gemm_eigen: utilisation d'une librairie externe
 */
void gemm_eigen(const Matrix& A, const Matrix& B, Matrix& C);
#endif
