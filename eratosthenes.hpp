//
// Created by ashy5000 on 6/29/25.
//

#ifndef ERATOSTHENES_HPP
#define ERATOSTHENES_HPP
#include <vector>
#include <iostream>

inline std::vector<unsigned int> sieve(unsigned int n) {
  std::vector<bool> ints;
  ints.reserve(n - 1);
  for (unsigned int i = 2; i <= n; i++) {
    ints.push_back(true);
  }
  unsigned int res_len = n - 2;
  for (unsigned int p = 2; p * p <= n; p++) {
    if (ints[p - 2]) {
      for (unsigned int i = p * p; i <= n; i += p) {
        ints[i - 2] = false;
        res_len--;
      }
    }
  }
  std::vector<unsigned int> res;
  res.reserve(res_len);
  for (unsigned int i = 0; i < ints.size(); i++) {
    if (ints[i]) {
      res.push_back(i + 2);
    }
  }
  return res;
}

#endif //ERATOSTHENES_HPP
