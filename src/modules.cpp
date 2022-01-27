#include "modules.h"

const std::vector<struct gemm_klass> algo = {{"basic", gemm_basic},
                                             {"simd", gemm_simd},
                                             {"blocked", gemm_blocked},
                                             {"omp", gemm_omp},
                                             {"combo", gemm_combo}
#ifdef HAS_EIGEN
                                             ,
                                             {"eigen", gemm_eigen}
#endif
};
