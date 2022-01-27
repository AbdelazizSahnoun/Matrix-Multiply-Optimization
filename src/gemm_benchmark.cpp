#include <chrono>
#include <fstream>

#include "modules.h"

double benchmark(const std::function<void()>& func, size_t repeat = 3) {

  // Execute une fois hors de la mesure pour réchauffer
  func();



  const auto t1 = std::chrono::steady_clock::now();
  for (size_t i = 0; i < repeat; i++) {
    const auto start = std::chrono::steady_clock::now();
    func();
    const auto end = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(end - start).count();
    std::cout << "progress " << (i + 1) << "/" << repeat << " " << elapsed << std::endl;
  }
  const auto t2 = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(t2 - t1).count() / repeat;
}

int main(int argc, char** argv) {

  // premier argument est la taille
  size_t n = 1024;
  if (argc > 1) {
    n = std::atol(argv[1]);
  }

 Matrix m1(n);
 m1.fill_random();


 Matrix m2(n);
 m2.fill_random();


 Matrix m3(n);
 m3.fill_zero();

 auto retour=benchmark(
     [&]{
       gemm_basic(m1,m2,m3);
     }
     );
  std::cout << "Basic temps moyen: \n" << retour << std::endl;

  retour=benchmark(
               [&]{
                 gemm_blocked(m1,m2,m3);
               }
               );
  std::cout << "Blocked temps moyen: \n"<< retour << std::endl;
  retour=benchmark(
      [&]{
        gemm_simd(m1,m2,m3);
      }
  );
 std::cout<< "Simd temps moyen: \n"<< retour << std::endl;


 retour=benchmark(
              [&]{
                gemm_omp(m1,m2,m3);
              });
 std::cout<< "Omp temps moyen: \n"<< retour << std::endl;

 retour=benchmark(
     [&]{
       gemm_combo(m1,m2,m3);
     });
 std::cout<< "Combo temps moyen: \n"<< retour << std::endl;


  // À COMPLÉTER

  return 0;
}
