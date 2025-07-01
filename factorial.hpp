//
// Created by ashy5000 on 6/29/25.
//

#ifndef FACTORIAL_CUH
#define FACTORIAL_CUH
#include <vector>
#include <omp.h>

#include "eratosthenes.hpp"
#include "math.hpp"

inline std::vector<uint> power_from_prime(const uint n, const uint p) {
  uint exp = 0;
  uint power = p;
  while (power <= n) {
    exp += n / power;
    power *= p;
  }
  std::vector<uint> big_p = {p};
  std::vector<uint> prime_power = exp_by_sqr(big_p, exp);
  return prime_power;
}

inline void combine_intermediates(std::vector<std::vector<uint>> &intermediates) {
  while (intermediates.size() > 1) {
    if(intermediates.size() % 2 != 0) {
      intermediates.push_back({1});
    }
    for (uint i = 0; i < intermediates.size(); i++) {
      intermediates.insert(intermediates.begin() + i, multiply(intermediates[i], intermediates[i + 1]));
      intermediates.erase(intermediates.begin() + i + 1, intermediates.begin() + i + 3);
    }
  }
}

inline std::vector<std::vector<uint>> combine_intermediates_multithreaded(std::vector<std::vector<uint>> intermediates) {
  if (intermediates.size() == 1) {
    return intermediates;
  }
  // int num_threads = omp_get_max_threads();
  int num_threads = omp_get_max_threads();
  if (intermediates.size() < num_threads) {
    combine_intermediates(intermediates);
    return intermediates;
  }
  omp_set_num_threads(num_threads);
  const std::vector<uint> empty = {};
#pragma omp parallel
  {
    const uint n = omp_get_thread_num();
    std::vector<std::vector<uint>> subsection = {};
    for (uint i = n; i < intermediates.size(); i += num_threads) {
      subsection.push_back(intermediates[i]);
    }
    combine_intermediates_multithreaded(subsection);
    intermediates[n] = subsection[0];
  }
  intermediates.resize(num_threads);
  combine_intermediates(intermediates);
  return intermediates;
}

inline std::vector<uint> factorial(const uint n) {
  std::vector<uint> primes = sieve(n);
  std::vector<std::vector<uint>> intermediates = {};
  intermediates.reserve(primes.size() + 1);
  for (const uint p : primes) {
    std::vector<uint> prime_power = power_from_prime(n, p);
    intermediates.push_back(prime_power);
  }
  return combine_intermediates_multithreaded(intermediates)[0];
}

#endif //FACTORIAL_CUH
