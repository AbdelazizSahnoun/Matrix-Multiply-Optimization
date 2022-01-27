add_executable(test_gemm test_gemm.cpp)
target_link_libraries(test_gemm PRIVATE gemmlib)

add_test(NAME test_gemm COMMAND test_gemm)
