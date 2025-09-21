#include <gtest/gtest.h>
#include "CreatingThreadWinAPI32.h"
#include <vector>
#include <cmath>

TEST(ThreadTest, CalculatesCorrectSum) {
    std::vector<int> input = {1, 4, 9};
    double result = 0;
    runThread(0, input, result);
    EXPECT_NEAR(result, sqrt(1) + sqrt(4) + sqrt(9), 0.001);
}
