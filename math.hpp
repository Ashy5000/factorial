//
// Created by ashy5000 on 6/28/25.
//

#ifndef MATH_HPP
#define MATH_HPP
#include <algorithm>
#include <vector>
#include <tuple>
#include <cstdlib>
#include <cstring>
#include <iostream>

typedef unsigned long long int uint64;

class BigInt {
public:
  static const unsigned int THRESHOLD = 2;
  uint64 *data = nullptr;
  size_t current_capacity = 0;
  size_t max_capacity = 0;

    void reserve(size_t size_p) {
      size_t size = size_p * sizeof(uint64);
      if (size <= max_capacity || size == 0) {
        return;
      }
      uint64* new_ptr;
      if (data) {
        new_ptr = (uint64*)(std::realloc(data, size));
      } else {
        new_ptr = (uint64*)(std::malloc(size));
      }
      if (!new_ptr) {
        std::cout << "FATAL ERROR: allocation failed!" << std::endl;
        exit(1);
      }
      data = new_ptr;
      for (size_t i = max_capacity; i < size; i += sizeof(uint64)) {
        data[i / sizeof(uint64)] = 0;
      }
      max_capacity = size;
    }

    void pad(size_t target_length_p) {
      size_t target_length = target_length_p * sizeof(uint64);
      if (target_length <= current_capacity) {
        return;
      }
      reserve(target_length_p);
      size_t offset = (target_length - current_capacity) / sizeof(uint64);
      memmove(data + offset, data, current_capacity);
      for (size_t i = 0; i < offset; i++) {
        data[i] = 0;
      }
      current_capacity = target_length;
    }

    void remove(size_t pos) {
      if (pos >= elems()) {
        return;
      }
      size_t elements_after = elems() - pos - 1;
      memmove(data + pos, data + pos + 1, elements_after * sizeof(uint64));
      current_capacity -= sizeof(uint64);
    }

    void push_back(uint64 val) {
      current_capacity += sizeof(uint64);
      if(current_capacity > max_capacity) {
        reserve(current_capacity / sizeof(uint64));
      }
      data[current_capacity / sizeof(uint64) - 1] = val;
    }

    void set(size_t rel_pos, uint64 val) {
      data[rel_pos] = val;
    }

    void inc_len() {
        current_capacity += sizeof(uint64);
    }

    void compliment() {
      for(size_t i = 0; i < elems(); i++) {
        set(i, ~data[i]);
      }
    }

    BigInt grade_school_mul(const BigInt& other) const {
      BigInt res = BigInt(0);
      for (unsigned int i = 0; i < elems(); i++) {
        for (unsigned int j = 0; j < other.elems(); j++) {
          unsigned int num_zeroes = (elems() - i - 1) + (other.elems() - j - 1);
          __uint128_t a = data[i];
          __uint128_t b = other.data[j];
          __uint128_t c = a * b;
          uint64 low_part = (uint64)c;
          uint64 high_part = (uint64)(c >> 64);
          BigInt term;
          if (high_part > 0) {
            term.reserve(num_zeroes + 2);
            term.push_back(high_part);
            term.push_back(low_part);
          } else {
            term.reserve(num_zeroes + 1);
            term.push_back(low_part);
          }
          for (unsigned int k = 0; k < num_zeroes; k++) {
            term.push_back(0);
          }
          res += term;
        }
      }
      return res;
    }

    std::pair<BigInt*, BigInt*> split(size_t chop) const {
      size_t s = elems();
      if (s > chop) {
        BigInt* a = new BigInt();
        a->reserve(chop);
        for (size_t i = 0; i < chop; i++) {
          a->data[i] = data[i];
        }
        a->current_capacity = chop * sizeof(uint64);

        BigInt* b = new BigInt();
        b->reserve(s - chop);
        for (size_t i = 0; i < s - chop; i++) {
          b->data[i] = data[i + chop];
        }
        b->current_capacity = (s - chop) * sizeof(uint64);

        return std::make_pair(b, a);
      } else {
        BigInt* copy = new BigInt();
        copy->reserve(s);
        copy->current_capacity = current_capacity;
        for (size_t i = 0; i < s; i++) {
          copy->data[i] = data[i];
        }
        BigInt* zero = new BigInt(0);
        return std::make_pair(copy, zero);
      }
    }

    BigInt karatsuba(const BigInt& other) const {
      size_t a_size = elems();
      size_t b_size = other.elems();
      if (a_size < THRESHOLD || b_size < THRESHOLD) {
        return grade_school_mul(other);
      }
      const size_t max_size = std::min(a_size, b_size);
      const size_t chop = max_size / 2;
      std::pair<BigInt*, BigInt*> sections0 = split(chop);
      BigInt* low1 = std::get<0>(sections0);
      BigInt* high1 = std::get<1>(sections0);
      std::pair<BigInt*, BigInt*> sections1 = other.split(chop);
      BigInt* low2 = std::get<0>(sections1);
      BigInt* high2 = std::get<1>(sections1);
      auto sum1 = *low1 + *high1;
      auto sum2 = *low2 + *high2;
      const BigInt z0 = *low1 * *low2;
      const BigInt z1 = sum1 * sum2;
      const BigInt z2 = *high1 * *high2;
      delete low1;
      delete high1;
      delete low2;
      delete high2;
      BigInt term0 = z2;
      term0.reserve(term0.elems() + chop * 2);
      for (unsigned int i = 0; i < chop * 2; i++) {
        term0.push_back(0);
      }
      BigInt sub0 = z1 - z2;
      BigInt sub1 = sub0 - z0;
      BigInt term1 = sub1;
      term1.reserve(term1.elems() + chop);
      for (unsigned int i = 0; i < chop; i++) {
        term1.push_back(0);
      }
      BigInt term2 = z0;
      BigInt res = term0 + term1 + term2;
      return res;
    }

public:
    BigInt() {
      data = nullptr;
      max_capacity = 0;
      current_capacity = 0;
    }

    BigInt(uint64* data_p, size_t cap, size_t current) {
      data = data_p;
      max_capacity = cap;
      current_capacity = current;
    }

    BigInt(uint64 x) {
      current_capacity = 0;
      max_capacity = 0;
      data = nullptr;
      reserve(1);
      *data = x;
      current_capacity = sizeof(uint64);
    }

    BigInt(const BigInt& other) {
      data = nullptr;
      current_capacity = 0;
      max_capacity = 0;
      if (other.data && other.current_capacity > 0) {
        reserve(other.current_capacity / sizeof(uint64));
        memcpy(data, other.data, other.current_capacity);
      }
      current_capacity = other.current_capacity;
    }

    BigInt& operator=(const BigInt& other) {
      if (this != &other) {
        data = nullptr;
        current_capacity = 0;
        max_capacity = 0;
        if (other.data && other.current_capacity > 0) {
          reserve(other.current_capacity / sizeof(uint64));
          memcpy(data, other.data, other.current_capacity);
        }
        current_capacity = other.current_capacity;
      }
      return *this;
    }

    ~BigInt() {
      if (data) {
        free(data);
        data = nullptr;
      }
    }

    size_t elems() const {
      return current_capacity / sizeof(uint64);
    }

    BigInt operator+(BigInt const& other) const {
      const size_t a_size = elems();
      const size_t b_size = other.elems();
      const size_t min_size = std::min(a_size, b_size);
      const size_t max_size = std::max(a_size, b_size);
      auto min_vec = min_size == a_size ? data : other.data;
      auto max_vec = min_size != a_size ? data : other.data;
      BigInt res = BigInt();
      res.reserve(max_size);
      size_t pos = max_size - 1;
      unsigned int carry = 0;
      for (unsigned int i = 1; i <= min_size; i++) {
        uint64 new_carry = 0;
        uint64 seg_min = min_vec[min_size - i];
        uint64 seg_max = max_vec[max_size - i];
        uint64 sum_0;
        if (__builtin_uaddll_overflow(seg_min, seg_max, &sum_0)) {
          new_carry = 1;
        }
        uint64 sum_1;
        if (__builtin_uaddll_overflow(sum_0, carry, &sum_1)) {
          new_carry += 1;
        }
        res.data[pos] = sum_1;
        pos--;
        carry = new_carry;
      }
      for(int i = max_size - min_size - 1; i >= 0; i -= 1) {
        uint64 max_seg = max_vec[i];
        if (__builtin_uaddll_overflow(max_seg, carry, &max_seg)) {
          carry = 1;
        } else {
          carry = 0;
        }
        res.data[pos] = max_seg;
        pos--;
      }
      res.current_capacity = max_size * sizeof(uint64);
      if (carry > 0) {
        res.reserve(max_size + 1);
        memmove(res.data + 1, res.data, res.current_capacity);
        res.data[0] = carry;
        res.current_capacity += sizeof(uint64);
      }
      return res;
    }

    void operator+=(const BigInt& other) {
        *this = *this + other;
    }

    BigInt operator++(int) {
        *this += BigInt(1);
        return *this;
    }

    BigInt operator-(const BigInt& other) const {
      BigInt self = *this;
      BigInt other_compliment = other;
      size_t largest_size = std::max(elems(), other_compliment.elems());
      self.pad(largest_size);
      other_compliment.pad(largest_size);
      other_compliment.compliment();
      BigInt res = self + other_compliment;
      if (res.elems() > elems() || res.elems() > other_compliment.elems()) {
        res.remove(0);
      }
      res++;
      return res;
    }

    BigInt operator*(BigInt& other) {
      // if (elems() < THRESHOLD || other.elems() < THRESHOLD) {
        return grade_school_mul(other);
      // }
      // return karatsuba(other);
    }

    void operator*=(BigInt other) {
        *this = *this * other;
    }

    BigInt operator^(unsigned long long int power) {
      if (power == 0) {
        return BigInt(1);
      }
      if (power == 1) {
          return *this;
      }
      BigInt base_sqr = *this * *this;
      if (power % 2 == 0) {
        return base_sqr ^ (power / 2);
      }
      BigInt recurse_res = base_sqr ^ ((power - 1) / 2);
      return *this * recurse_res;
    }
};

#endif //MATH_HPP
