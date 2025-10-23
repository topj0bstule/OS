#include <iostream>
#include <vector>
#include <stdexcept>
#include <limits>

class FactorialCalculator {
public:
    static std::vector<long long int> calculateFirstNFactorials(short int n) {
        std::vector<long long int> result;
        if (n == 0)
            return result;
        
        validateInput(n);
        result.reserve(n);
        long long int factorial = 1;
        result.push_back(factorial);
        
        for (size_t i = 1; i < n; ++i) {
            checkMultiplicationOverflow(factorial, i);
            factorial *= i;
            result.push_back(factorial);
        }
        
        return result;
    }

private:
    static void validateInput(int n) {
        if (n > MAX_FACTORIAL_INDEX)
            throw std::overflow_error("Requested number of factorials exceeds maximum computable value");
    }
    
    static void checkMultiplicationOverflow(long long int a, long long int b) {
        if (a > std::numeric_limits<long long int>::max() / b) {
            throw std::overflow_error("Factorial calculation would overflow");
        }
    }
    
    static constexpr short MAX_FACTORIAL_INDEX = 20;
};