//
// Created by ashy5000 on 6/28/25.
//

#include <iostream>

#include "perf.hpp"
#include "math.hpp"

int main() {
    // BigInt res = factorial(1000);
    // for (size_t i = 0; i < res.elems(); i++) {
    //     std::cout << res.data[i] << std::endl;
    // }

    std::cout << factorial_perf(5000) << std::endl;
}
