//
// Created by ashy5000 on 6/29/25.
//

#ifndef PERF_HPP
#define PERF_HPP
#include <chrono>
#include <iostream>

#include "factorial.hpp"

inline long factorial_perf(unsigned int n) {
  auto start = std::chrono::high_resolution_clock::now();
  factorial(n);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  return duration.count();
}

inline unsigned int perf() {
  unsigned int n = 100;
  unsigned int inc = 100;
  for (;;) {
    unsigned int res = factorial_perf(n);
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

#endif //PERF_HPP
