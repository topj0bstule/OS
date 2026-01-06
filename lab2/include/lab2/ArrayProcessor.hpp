#pragma once

#include <windows.h>
#include <iostream>
#include <vector>

namespace lab2 {

    class ArrayProcessor {
        public:
            static std::vector<int> GenerateRandomArray(int size);
            static void PrintArray(const std::vector<int>& arr);
            static double CalculateAverage(const std::vector<int>& arr);
            
            static bool ReadArraySizeFromPipe(HANDLE hPipe, int& size);
            static bool ReadArrayFromPipe(HANDLE hPipe, std::vector<int>& arr, int size);
            static bool WriteArraySizeToPipe(HANDLE hPipe, int size);
            static bool WriteArrayToPipe(HANDLE hPipe, const std::vector<int>& arr);
            static bool ReadDoubleFromPipe(HANDLE hPipe, double& value);
            static bool WriteDoubleToPipe(HANDLE hPipe, double value);
    };

}