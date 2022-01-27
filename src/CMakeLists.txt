add_library(gemmlib
  modules.cpp
  modules.h
  gemm.cpp
  gemm.h
  matrix.cpp
  matrix.h
)
target_link_libraries(gemmlib PUBLIC Threads::Threads OpenMP::OpenMP_CXX)
if (ENABLE_EIGEN)
  target_link_libraries(gemmlib PUBLIC Eigen3::Eigen)
  target_compile_definitions(gemmlib PUBLIC -DHAS_EIGEN)
endif()
target_include_directories(gemmlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

add_executable(gemm main.cpp)
target_link_libraries(gemm gemmlib)

add_executable(gemm_benchmark gemm_benchmark.cpp)
target_link_libraries(gemm_benchmark gemmlib)
