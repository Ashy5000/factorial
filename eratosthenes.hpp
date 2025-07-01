//
// Created by ashy5000 on 6/29/25.
//

#ifndef ERATOSTHENES_CUH
#define ERATOSTHENES_CUH
#include <vector>

inline std::vector<uint> sieve(uint n) {
  std::vector<bool> ints;
  ints.reserve(n - 2);
  for (int i = 2; i < n; i++) {
    ints.push_back(true);
  }
  uint res_len = n - 2;
  for (int p = 2; p * p <= n; p++) {
    if (ints[p - 2]) {
      for (int i = p * p; i <= n; i += p) {
        ints[i - 2] = false;
        res_len--;
      }
    }
  }
  std::vector<uint> res;
  res.reserve(res_len);
  for (uint i = 0; i < ints.size(); i++) {
    if (ints[i]) {
      res.push_back(i + 2);
    }
  }
  return res;
}

#endif //ERATOSTHENES_CUH
