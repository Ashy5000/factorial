//
// Created by ashy5000 on 6/29/25.
//

#ifndef FACTORIAL_HPP
#define FACTORIAL_HPP
#include <vector>
#include <omp.h>

#include "eratosthenes.hpp"
#include "math.hpp"

const unsigned int MIN_THREADS = 4;

inline std::vector<unsigned int> power_from_prime(const unsigned int n, const unsigned int p) {
  unsigned int exp = 0;
  unsigned int power = p;
  while (power <= n) {
    exp += n / power;
    power *= p;
  }
  std::vector<unsigned int> big_p = {p};
  std::vector<unsigned int> prime_power = exp_by_sqr(big_p, exp);
  return prime_power;
}

inline void combine_intermediates(std::vector<std::vector<unsigned int>> &intermediates) {
  while (intermediates.size() > 1) {
    if(intermediates.size() % 2 != 0) {
      intermediates.push_back({1});
    }
    for (unsigned int i = 0; i < intermediates.size(); i++) {
      intermediates.insert(intermediates.begin() + i, multiply(intermediates[i], intermediates[i + 1]));
      intermediates.erase(intermediates.begin() + i + 1, intermediates.begin() + i + 3);
    }
  }
}

inline std::vector<std::vector<unsigned int>> combine_intermediates_multithreaded(std::vector<std::vector<unsigned int>> intermediates) {
  if (intermediates.size() == 1) {
    return intermediates;
  }
  // int num_threads = omp_get_max_threads();
  unsigned int num_threads = std::min(omp_get_max_threads(), static_cast<int>(intermediates.size()) / 2);
  if (num_threads < MIN_THREADS) {
    combine_intermediates(intermediates);
    return intermediates;
  }
  omp_set_num_threads(num_threads);
  const std::vector<unsigned int> empty = {};
#pragma omp parallel
  {
    const unsigned int n = omp_get_thread_num();
    std::vector<std::vector<unsigned int>> subsection = {};
    for (unsigned int i = n; i < intermediates.size(); i += num_threads) {
      subsection.push_back(intermediates[i]);
    }
    combine_intermediates_multithreaded(subsection);
    intermediates[n] = subsection[0];
  }
  intermediates.resize(num_threads);
  combine_intermediates_multithreaded(intermediates);
  return intermediates;
}

inline std::vector<unsigned int> factorial(const unsigned int n) {
  std::vector<unsigned int> primes = sieve(n);
  std::vector<std::vector<unsigned int>> intermediates = {};
  intermediates.reserve(primes.size() + 1);
  for (const unsigned int p : primes) {
    std::vector<unsigned int> prime_power = power_from_prime(n, p);
    intermediates.push_back(prime_power);
  }
  return combine_intermediates_multithreaded(intermediates)[0];
}

#endif //FACTORIAL_HPP
