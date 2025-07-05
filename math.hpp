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

typedef unsigned long long int seg;
typedef __uint128_t double_seg;
#define SEG_BITS 64

class BigInt {
public:
  static const unsigned int THRESHOLD = 8;
  seg *data = nullptr;
  size_t current_capacity = 0;
  size_t max_capacity = 0;

    void reserve(size_t size_p) {
      size_t size = size_p * sizeof(seg);
      if (size <= max_capacity || size == 0) {
        return;
      }
      seg* new_ptr;
      if (data) {
        new_ptr = (seg*)(std::realloc(data, size));
      } else {
        new_ptr = (seg*)(std::malloc(size));
      }
      if (!new_ptr) {
        std::cout << "FATAL ERROR: allocation failed!" << std::endl;
        exit(1);
      }
      data = new_ptr;
      for (size_t i = max_capacity; i < size; i += sizeof(seg)) {
        data[i / sizeof(seg)] = 0;
      }
      max_capacity = size;
    }

    void pad(size_t target_length_p) {
      size_t target_length = target_length_p * sizeof(seg);
      if (target_length <= current_capacity) {
        return;
      }
      reserve(target_length_p);
      size_t offset = (target_length - current_capacity) / sizeof(seg);
      for (size_t i = 0; i < offset; i++) {
        push_back(0);
      }
    }

    void remove(size_t pos) {
      if (pos >= elems()) {
        return;
      }
      size_t elements_after = elems() - pos - 1;
      memmove(data + pos, data + pos + 1, elements_after * sizeof(seg));
      current_capacity -= sizeof(seg);
    }

    void push_back(seg val) {
      current_capacity += sizeof(seg);
      if(current_capacity > max_capacity) {
        reserve(current_capacity / sizeof(seg));
      }
      data[current_capacity / sizeof(seg) - 1] = val;
    }

    void set(size_t rel_pos, seg val) {
      data[rel_pos] = val;
    }

    void inc_len() {
        current_capacity += sizeof(seg);
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
          unsigned int num_zeroes = i + j;
          double_seg a = data[i];
          double_seg b = other.data[j];
          double_seg c = a * b;
          seg low_part = (seg)c;
          seg high_part = (seg)(c >> SEG_BITS);
          BigInt term;
          if (high_part > 0) {
            term.reserve(num_zeroes + 2);
          } else {
            term.reserve(num_zeroes + 1);
          }
          for (unsigned int k = 0; k < num_zeroes; k++) {
            term.push_back(0);
          }
          if (high_part > 0) {
            term.push_back(low_part);
            term.push_back(high_part);
          } else {
            term.push_back(low_part);
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
        a->current_capacity = chop * sizeof(seg);

        BigInt* b = new BigInt();
        b->reserve(s - chop);
        for (size_t i = 0; i < s - chop; i++) {
          b->data[i] = data[i + chop];
        }
        b->current_capacity = (s - chop) * sizeof(seg);

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

    BigInt(seg* data_p, size_t cap, size_t current) {
      data = data_p;
      max_capacity = cap;
      current_capacity = current;
    }

    BigInt(seg x) {
      current_capacity = 0;
      max_capacity = 0;
      data = nullptr;
      reserve(1);
      *data = x;
      current_capacity = sizeof(seg);
    }

    BigInt(const BigInt& other) {
      data = nullptr;
      current_capacity = 0;
      max_capacity = 0;
      if (other.data && other.current_capacity > 0) {
        reserve(other.current_capacity / sizeof(seg));
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
          reserve(other.current_capacity / sizeof(seg));
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
      return current_capacity / sizeof(seg);
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
      unsigned int carry = 0;
      for (unsigned int i = 0; i < min_size; i++) {
        seg new_carry = 0;
        seg seg_min = min_vec[i];
        seg seg_max = max_vec[i];
        seg sum_0;
        if (__builtin_add_overflow(seg_min, seg_max, &sum_0)) {
          new_carry = 1;
        }
        seg sum_1;
        if (__builtin_add_overflow(sum_0, carry, &sum_1)) {
          new_carry += 1;
        }
        res.push_back(sum_1);
        carry = new_carry;
      }
      for(unsigned int i = min_size; i < max_size; i++) {
        seg max_seg = max_vec[i];
        if (__builtin_add_overflow(max_seg, carry, &max_seg)) {
          carry = 1;
        } else {
          carry = 0;
        }
        res.push_back(max_seg);
      }
      if (carry > 0) {
        res.push_back(carry);
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
