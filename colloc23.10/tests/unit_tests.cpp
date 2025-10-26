#include <gtest/gtest.h>
#include "dubl.hpp"
#include "fact.hpp"
#include "list.hpp"

TEST(DuplicateRemoverTest, IntVector) {
    std::vector<int> input = {1, 2, 2, 3, 1, 4};
    std::vector<int> expected = {1, 2, 3, 4};
    EXPECT_EQ(DuplicateRemover<int>::removeDuplicatesPreserveOrder(input), expected);
}

TEST(DuplicateRemoverTest, StringVector) {
    std::vector<std::string> input = {"a", "b", "a", "c"};
    std::vector<std::string> expected = {"a", "b", "c"};
    EXPECT_EQ(DuplicateRemover<std::string>::removeDuplicatesPreserveOrder(input), expected);
}

TEST(FactorialCalculatorTest, ValidInput) {
    std::vector<long long> expected = {1, 1, 2, 6, 24};
    EXPECT_EQ(FactorialCalculator::calculateFirstNFactorials(5), expected);
}

TEST(FactorialCalculatorTest, ZeroInput) {
    EXPECT_TRUE(FactorialCalculator::calculateFirstNFactorials(0).empty());
}

TEST(FactorialCalculatorTest, OverflowInput) {
    EXPECT_THROW(FactorialCalculator::calculateFirstNFactorials(21), std::overflow_error);
}

TEST(LinkedListTest, AddAndReverse) {
    LinkedList<int> list;
    EXPECT_TRUE(list.isEmpty());

    list.add(10);
    list.add(20);
    list.add(30);
    EXPECT_EQ(list.getSize(), 3);

    auto head = list.getHead();
    EXPECT_EQ(head->data, 10);
    EXPECT_EQ(head->next->data, 20);
    EXPECT_EQ(head->next->next->data, 30);

    list.reverse();
    head = list.getHead();
    EXPECT_EQ(head->data, 30);
    EXPECT_EQ(head->next->data, 20);
    EXPECT_EQ(head->next->next->data, 10);
}
