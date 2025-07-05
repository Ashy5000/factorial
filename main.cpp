//
// Created by ashy5000 on 6/28/25.
//

#include <iostream>
#include <iomanip>

#include "perf.hpp"
#include "math.hpp"

int main() {
    // BigInt res = factorial(1000);
    // for (size_t i = res.elems() - 1; i > 0; --i) {
    //     std::cout << std::setw(8) << std::setfill('0') << std::hex << res.data[i];
    // }
    // std::cout << std::endl;

    std::cout << factorial_perf(5000) << std::endl;
}
