#pragma once

#include <windows.h>
#include <vector>

struct ThreadData {
    int* Numbers;
    int size;
    double result;
};

DWORD WINAPI WorkerThread(LPVOID LpParam);

void printNumbers(std::vector<int>& num, int& size);