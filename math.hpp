//
// Created by ashy5000 on 6/28/25.
//

#ifndef MATH_CUH
#define MATH_CUH
#include <algorithm>
#include <vector>

inline std::vector<uint> add(const uint a_size, const std::vector<uint>::const_iterator a, const uint b_size, const std::vector<uint>::const_iterator b) {
  const uint min_size = std::min(a_size, b_size);
  const uint max_size = std::max(a_size, b_size);
  std::vector<uint>::const_iterator min_iter = min_size == a_size ? a : b;
  std::vector<uint>::const_iterator max_iter = min_size != a_size ? a : b;
  std::vector<uint> res = {};
  res.reserve(max_size);
  uint carry = 0;
  for (int i = 1; i < min_size + 1; i++) {
    uint new_carry = 0;
    uint seg_min = *(min_iter + min_size - i);
    uint seg_max = *(max_iter + max_size - i);
    uint sum_0;
    if (__builtin_add_overflow(seg_min, seg_max, &sum_0)) {
      new_carry = 1;
    }
    uint sum_1;
    if (__builtin_add_overflow(sum_0, carry, &sum_1)) {
      new_carry += 1;
    }
    res.push_back(sum_1);
    carry = new_carry;
  }
  for (int i = 1; i <= max_size - min_size; i++) {
    uint seg = *(max_iter + max_size - min_size - i);
    if (carry > 0) {
      if (__builtin_add_overflow(seg, carry, &seg)) {
        carry = 1;
      } else {
        carry = 0;
      }
    }
    res.push_back(seg);
  }
  if (carry > 0) {
    res.push_back(carry);
  }
  std::reverse(res.begin(), res.end());
  return res;
}

inline std::vector<uint> compliment(const uint x_size, const std::vector<uint>::const_iterator x) {
  std::vector<uint> res = {};
  res.reserve(x_size);
  for (int i = 0; i < x_size; i++) {
    res.push_back(std::numeric_limits<uint>::max() - *(x + i));
  }
  return res;
}

inline std::vector<uint> sub(const std::vector<uint> a, const std::vector<uint> b) {
  std::vector<uint> b_compliment = compliment(b.size(), b.begin());
  std::vector<uint> a_padded = a;
  std::vector<uint> b_padded = b_compliment;
  if (a.size() < b.size()) {
    for (int i = a.size(); i < b.size(); i++) {
      a_padded.insert(a_padded.begin(), 0);
    }
  } else if (b.size() < a.size()) {
    for (int i = b.size(); i < a.size(); i++) {
      b_padded.insert(b_padded.begin(), 0);
    }
  }
  std::vector<uint> res = add(a_padded.size(), a_padded.begin(), static_cast<uint>(b_padded.size()), b_padded.begin());
  res.erase(res.begin());
  if (res.size() > 0) {
    res[res.size() - 1] += 1;
  }
  return res;
}

inline std::vector<uint> grade_school_mul(const uint a_size, const std::vector<uint>::const_iterator a, const uint b_size, const std::vector<uint>::const_iterator b) {
  std::vector<uint> res = {0};
  for (uint i = 0; i < a_size; i++) {
    for (uint j = 0; j < b_size; j++) {
      uint num_zeroes = (a_size - i - 1) + (b_size - j - 1);
      uint mul_res;
      bool carry = false;
      if (__builtin_mul_overflow(*(a + i), *(b + j), &mul_res)) {
        carry = true;
      }
      std::vector<uint> term = {};
      term.reserve(num_zeroes + 2);
      if (carry) {
        term.push_back(1);
      }
      term.push_back(mul_res);
      for (uint k = 0; k < num_zeroes; k++) {
        term.push_back(0);
      }
      res = add(static_cast<uint>(res.size()), res.begin(), static_cast<uint>(term.size()), term.begin());
    }
  }
  return res;
}

inline std::vector<uint> karatsuba(const uint a_size, const std::vector<uint>::const_iterator a, const uint b_size, const std::vector<uint>::const_iterator b) {
  if (a_size == 1 || b_size == 1) {
    return grade_school_mul(a_size, a, b_size, b);
  }
  const uint max_size = std::max(a_size, b_size);
  const uint chop = max_size / 2;
  auto high1 = a;
  std::vector<uint> zero_0 = {0};
  std::vector<uint> zero_1 = {0};
  uint high1_len = a_size - std::min(a_size, chop);
  const auto low1 = a_size > chop ? a + a_size - chop : a;
  const uint low1_len = a_size - high1_len;
  if (high1_len == 0) {
    high1_len = 1;
    high1 = zero_0.begin();
  }
  auto high2 = b;
  uint high2_len = b_size - std::min(b_size, chop);
  const auto low2 = b_size > chop ? b + b_size - chop : b;
  const uint low2_len = b_size - high2_len;
  if (high2_len == 0) {
    high2_len = 1;
    high2 = zero_1.begin();
  }
  auto sum1 = add(low1_len, low1, high1_len, high1);
  auto sum2 = add(low2_len, low2, high2_len, high2);
  const std::vector<uint> z0 = karatsuba(low1_len, low1, low2_len, low2);
  const std::vector<uint> z1 = karatsuba(static_cast<uint>(sum1.size()), sum1.begin(), static_cast<uint>(sum2.size()), sum2.begin());
  const std::vector<uint> z2 = karatsuba(high1_len, high1, high2_len, high2);
  std::vector<uint> term0 = z2;
  term0.reserve(term0.size() + chop * 2);
  for (int i = 0; i < chop * 2; i++) {
    term0.push_back(0);
  }
  std::vector<uint> sub0 = sub(z1, z2);
  std::vector<uint> sub1 = sub(sub0, z0);
  std::vector<uint> term1 = sub1;
  term1.reserve(term1.size() + chop);
  for (int i = 0; i < chop; i++) {
    term1.push_back(0);
  }
  std::vector<uint> term2 = z0;
  std::vector<uint> add0 = add(static_cast<uint>(term0.size()), term0.begin(), static_cast<uint>(term1.size()), term1.begin());
  std::vector<uint> add1 = add(static_cast<uint>(add0.size()), add0.begin(), static_cast<uint>(term2.size()), term2.begin());
  return add1;
}

inline std::vector<uint> multiply(std::vector<uint> &a, std::vector<uint> &b) {
  if (a.size() < 5 || b.size() < 5) {
    return grade_school_mul(static_cast<uint>(a.size()), a.begin(), static_cast<uint>(b.size()), b.begin());
  }
  return karatsuba(static_cast<uint>(a.size()), a.begin(), static_cast<uint>(b.size()), b.begin());
}

inline std::vector<uint> exp_by_sqr(std::vector<uint> &base, const uint power) {
  if (power == 0) {
    return {1};
  }
  std::vector<uint> base_sqr = multiply(base, base);
  if (power % 2 == 0) {
    return exp_by_sqr(base_sqr, power / 2);
  }
  std::vector<uint> recurse_res = exp_by_sqr(base_sqr, (power - 1) / 2);
  return multiply(base, recurse_res);
}

#endif //MATH_CUH
