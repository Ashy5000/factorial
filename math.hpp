//
// Created by ashy5000 on 6/28/25.
//

#ifndef MATH_HPP
#define MATH_HPP
#include <algorithm>
#include <vector>

const unsigned int THRESHOLD = 10;

inline std::vector<unsigned int> add(const size_t a_size, const std::vector<unsigned int>::const_iterator a, const size_t b_size, const std::vector<unsigned int>::const_iterator b) {
  const unsigned int min_size = std::min(a_size, b_size);
  const unsigned int max_size = std::max(a_size, b_size);
  std::vector<unsigned int>::const_iterator min_iter = min_size == a_size ? a : b;
  std::vector<unsigned int>::const_iterator max_iter = min_size != a_size ? a : b;
  std::vector<unsigned int> res = {};
  res.reserve(max_size);
  unsigned int carry = 0;
  for (unsigned int i = 1; i < min_size + 1; i++) {
    unsigned int new_carry = 0;
    unsigned int seg_min = *(min_iter + min_size - i);
    unsigned int seg_max = *(max_iter + max_size - i);
    unsigned int sum_0;
    if (__builtin_add_overflow(seg_min, seg_max, &sum_0)) {
      new_carry = 1;
    }
    unsigned int sum_1;
    if (__builtin_add_overflow(sum_0, carry, &sum_1)) {
      new_carry += 1;
    }
    res.push_back(sum_1);
    carry = new_carry;
  }
  for (unsigned int i = 1; i <= max_size - min_size; i++) {
    unsigned int seg = *(max_iter + max_size - min_size - i);
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

inline std::vector<unsigned int> compliment(const size_t x_size, const std::vector<unsigned int>::const_iterator x) {
  std::vector<unsigned int> res = {};
  res.reserve(x_size);
  for (unsigned int i = 0; i < x_size; i++) {
    res.push_back(0 - 1 - *(x + i));
  }
  return res;
}

inline std::vector<unsigned int> sub(const std::vector<unsigned int> a, const std::vector<unsigned int> b) {
  std::vector<unsigned int> b_compliment = compliment(b.size(), b.begin());
  std::vector<unsigned int> a_padded = a;
  std::vector<unsigned int> b_padded = b_compliment;
  if (a.size() < b.size()) {
    for (unsigned int i = a.size(); i < b.size(); i++) {
      a_padded.insert(a_padded.begin(), 0);
    }
  } else if (b.size() < a.size()) {
    for (unsigned int i = b.size(); i < a.size(); i++) {
      b_padded.insert(b_padded.begin(), 0);
    }
  }
  std::vector<unsigned int> res = add(a_padded.size(), a_padded.begin(), b_padded.size(), b_padded.begin());
  res.erase(res.begin());
  if (res.size() > 0) {
    res[res.size() - 1] += 1;
  }
  return res;
}

inline std::vector<unsigned int> grade_school_mul(const size_t a_size, const std::vector<unsigned int>::const_iterator a, size_t b_size, const std::vector<unsigned int>::const_iterator b) {
  std::vector<unsigned int> res = {0};
  for (unsigned int i = 0; i < a_size; i++) {
    for (unsigned int j = 0; j < b_size; j++) {
      unsigned int num_zeroes = (a_size - i - 1) + (b_size - j - 1);
      unsigned int mul_res;
      bool carry = false;
      if (__builtin_mul_overflow(*(a + i), *(b + j), &mul_res)) {
        carry = true;
      }
      std::vector<unsigned int> term = {};
      term.reserve(num_zeroes + 2);
      if (carry) {
        term.push_back(1);
      }
      term.push_back(mul_res);
      for (unsigned int k = 0; k < num_zeroes; k++) {
        term.push_back(0);
      }
      res = add(res.size(), res.begin(), term.size(), term.begin());
    }
  }
  return res;
}

inline std::vector<unsigned int> karatsuba(const size_t a_size, const std::vector<unsigned int>::const_iterator a, const size_t b_size, const std::vector<unsigned int>::const_iterator b) {
  if (a_size < THRESHOLD || b_size < THRESHOLD) {
    return grade_school_mul(a_size, a, b_size, b);
  }
  const size_t max_size = std::max(a_size, b_size);
  const size_t chop = max_size / 2;
  auto high1 = a;
  std::vector<unsigned int> zero_0 = {0};
  std::vector<unsigned int> zero_1 = {0};
  size_t high1_len = a_size - std::min(a_size, chop);
  const auto low1 = a_size > chop ? a + a_size - chop : a;
  const unsigned int low1_len = a_size - high1_len;
  if (high1_len == 0) {
    high1_len = 1;
    high1 = zero_0.begin();
  }
  auto high2 = b;
  size_t high2_len = b_size - std::min(b_size, chop);
  const auto low2 = b_size > chop ? b + b_size - chop : b;
  const unsigned int low2_len = b_size - high2_len;
  if (high2_len == 0) {
    high2_len = 1;
    high2 = zero_1.begin();
  }
  auto sum1 = add(low1_len, low1, high1_len, high1);
  auto sum2 = add(low2_len, low2, high2_len, high2);
  const std::vector<unsigned int> z0 = karatsuba(low1_len, low1, low2_len, low2);
  const std::vector<unsigned int> z1 = karatsuba(sum1.size(), sum1.begin(), sum2.size(), sum2.begin());
  const std::vector<unsigned int> z2 = karatsuba(high1_len, high1, high2_len, high2);
  std::vector<unsigned int> term0 = z2;
  term0.reserve(term0.size() + chop * 2);
  for (unsigned int i = 0; i < chop * 2; i++) {
    term0.push_back(0);
  }
  std::vector<unsigned int> sub0 = sub(z1, z2);
  std::vector<unsigned int> sub1 = sub(sub0, z0);
  std::vector<unsigned int> term1 = sub1;
  term1.reserve(term1.size() + chop);
  for (unsigned int i = 0; i < chop; i++) {
    term1.push_back(0);
  }
  std::vector<unsigned int> term2 = z0;
  std::vector<unsigned int> add0 = add(term0.size(), term0.begin(), term1.size(), term1.begin());
  std::vector<unsigned int> add1 = add(add0.size(), add0.begin(), term2.size(), term2.begin());
  return add1;
}

inline std::vector<unsigned int> multiply(std::vector<unsigned int> &a, std::vector<unsigned int> &b) {
  if (a.size() < THRESHOLD || b.size() < THRESHOLD) {
    return grade_school_mul(a.size(), a.begin(), b.size(), b.begin());
  }
  return karatsuba(a.size(), a.begin(), b.size(), b.begin());
}

inline std::vector<unsigned int> exp_by_sqr(std::vector<unsigned int> &base, const unsigned int power) {
  if (power == 0) {
    return {1};
  }
  std::vector<unsigned int> base_sqr = multiply(base, base);
  if (power % 2 == 0) {
    return exp_by_sqr(base_sqr, power / 2);
  }
  std::vector<unsigned int> recurse_res = exp_by_sqr(base_sqr, (power - 1) / 2);
  return multiply(base, recurse_res);
}

#endif //MATH_HPP
