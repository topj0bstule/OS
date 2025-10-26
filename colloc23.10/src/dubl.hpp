#pragma once
#include <vector>
#include <unordered_set>

template<typename T>
class DuplicateRemover {
public:
    static std::vector<T> removeDuplicatesPreserveOrder(const std::vector<T>& inputVec) {
        std::vector<T> result;
        std::unordered_set<T> seen;
        for (const auto& element : inputVec) {
            if (seen.insert(element).second) {
                result.push_back(element);
            }
        }
        return result;
    }
};
