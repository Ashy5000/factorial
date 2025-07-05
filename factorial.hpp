//
// Created by ashy5000 on 6/29/25.
//

#ifndef FACTORIAL_HPP
#define FACTORIAL_HPP
#include <vector>
#include <omp.h>

#include "eratosthenes.hpp"
#include "math.hpp"

const unsigned int MIN_THREADS = 2;

inline BigInt power_from_prime(const unsigned int n, const unsigned int p) {
  unsigned long long int exp = 0;
  unsigned long long int power = p;
  for (;;) {
    unsigned long long int frac = n / power;
    if (frac == 0) {
        break;
    } else {
        exp += frac;
    }
    power *= p;
  }
  BigInt big_p(p);
  BigInt prime_power = big_p ^ exp;
  return prime_power;
}

inline void combine_intermediates(std::vector<BigInt> &intermediates) {
  while (intermediates.size() > 1) {
    if(intermediates.size() % 2 != 0) {
      intermediates.push_back({1});
    }
    for (unsigned int i = 0; i < intermediates.size(); i++) {
      BigInt a = intermediates[i];
      BigInt b = intermediates[i + 1];
      BigInt c = a * b;
      intermediates.insert(intermediates.begin() + i, c);
      intermediates.erase(intermediates.begin() + i + 1, intermediates.begin() + i + 3);
    }
  }
}

inline std::vector<BigInt> combine_intermediates_multithreaded(std::vector<BigInt> intermediates) {
  if (intermediates.size() == 1) {
    return intermediates;
  }
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
    std::vector<BigInt> subsection = {};
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

inline BigInt factorial(const unsigned int n) {
  std::vector<unsigned int> primes = sieve(n);
  std::vector<BigInt> intermediates = {};
  intermediates.reserve(primes.size() + 1);
  for (const unsigned int p : primes) {
    BigInt prime_power = power_from_prime(n, p);
    intermediates.push_back(prime_power);
  }
  combine_intermediates(intermediates);
  BigInt res = intermediates[0];
  return res;
}

#endif //FACTORIAL_HPP
