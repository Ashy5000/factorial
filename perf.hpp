//
// Created by ashy5000 on 6/29/25.
//

#ifndef PERF_CUH
#define PERF_CUH
#include <chrono>

#include "factorial.hpp"

inline long factorial_perf(uint n) {
  auto start = std::chrono::high_resolution_clock::now();
  factorial(n);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return duration.count();
}

inline uint perf() {
  uint n = 100;
  uint inc = 100;
  for (;;) {
    uint res = factorial_perf(n);
    if (res > 1000000) {
      return n;
    }
    if (res > 500000) {
      inc = 30;
    }
    if (res > 700000) {
      inc = 10;
    }
    n += inc;
    std::cout << n << std::endl;
  }
}

#endif //PERF_CUH
