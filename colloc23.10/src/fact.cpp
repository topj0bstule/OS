#include "fact.hpp"
#include <stdexcept>
#include <limits>
#include <vector>

std::vector<long long int> FactorialCalculator::calculateFirstNFactorials(short int n) {
    std::vector<long long int> result;
    if (n == 0)
        return result;

    if (n > 20)
        throw std::overflow_error("Requested number of factorials exceeds maximum computable value");

    result.reserve(n);
    long long int factorial = 1;
    result.push_back(factorial);

    for (size_t i = 1; i < n; ++i) {
        if (factorial > std::numeric_limits<long long int>::max() / i)
            throw std::overflow_error("Factorial calculation overflow");
        factorial *= i;
        result.push_back(factorial);
    }

    return result;
}
