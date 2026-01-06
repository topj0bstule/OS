#include "lab2/ArrayProcessor.hpp"
#include <ctime>
#include <random>

namespace lab2 {

    std::vector<int> ArrayProcessor::GenerateRandomArray(int size) {
        std::vector<int> arr(size);
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        
        for (int i = 0; i < size; ++i) {
            arr[i] = std::rand() % 100;
        }
        
        return arr;
    }

    void ArrayProcessor::PrintArray(const std::vector<int>& arr) {
        std::cout << "Array: ";
        for (size_t i = 0; i < arr.size(); ++i) {
            std::cout << arr[i];
            if (i < arr.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }

    double ArrayProcessor::CalculateAverage(const std::vector<int>& arr) {
        if (arr.empty()) {
            return 0.0;
        }
        
        double sum = 0.0;
        for (int value : arr) {
            sum += value;
        }
        
        return sum / arr.size();
    }

    bool ArrayProcessor::ReadArraySizeFromPipe(HANDLE hPipe, int& size) {
        DWORD bytesRead = 0;
        return ReadFile(hPipe, &size, sizeof(size), &bytesRead, NULL) && 
            bytesRead == sizeof(size);
    }

    bool ArrayProcessor::ReadArrayFromPipe(HANDLE hPipe, std::vector<int>& arr, int size) {
        if (size <= 0) {
            arr.clear();
            return true;
        }
        
        arr.resize(size);
        DWORD bytesRead = 0;
        return ReadFile(hPipe, arr.data(), size * sizeof(int), &bytesRead, NULL) && 
            bytesRead == static_cast<DWORD>(size * sizeof(int));
    }

    bool ArrayProcessor::WriteArraySizeToPipe(HANDLE hPipe, int size) {
        DWORD bytesWritten = 0;
        return WriteFile(hPipe, &size, sizeof(size), &bytesWritten, NULL) && 
            bytesWritten == sizeof(size);
    }

    bool ArrayProcessor::WriteArrayToPipe(HANDLE hPipe, const std::vector<int>& arr) {
        if (arr.empty()) {
            return true;
        }
        
        DWORD bytesWritten = 0;
        return WriteFile(hPipe, arr.data(), arr.size() * sizeof(int), &bytesWritten, NULL) && 
            bytesWritten == static_cast<DWORD>(arr.size() * sizeof(int));
    }

    bool ArrayProcessor::ReadDoubleFromPipe(HANDLE hPipe, double& value) {
        DWORD bytesRead = 0;
        return ReadFile(hPipe, &value, sizeof(value), &bytesRead, NULL) && 
            bytesRead == sizeof(value);
    }

    bool ArrayProcessor::WriteDoubleToPipe(HANDLE hPipe, double value) {
        DWORD bytesWritten = 0;
        return WriteFile(hPipe, &value, sizeof(value), &bytesWritten, NULL) && 
            bytesWritten == sizeof(value);
    }

}