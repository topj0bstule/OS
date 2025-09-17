#include <iostream>
#include <windows.h>
#include <vector>
#include <cmath>

struct ThreadData {
    int* Numbers;
    int size;
    double* result;
};

DWORD WINAPI WorkerThread(LPVOID LpParam) {
    ThreadData* data = static_cast<ThreadData*>(LpParam);
    double sumOfSqrt = 0;
    for (int i = 0; i < data->size; i++) {
        sumOfSqrt += sqrt((double)data->Numbers[i]);
        Sleep(200);
    }
    *(data->result) = sumOfSqrt;
    return 0;
}

void printNumbers(std::vector<int>& num, int& size) {
    for (int i = 0; i < size; i++) {
        std::cout << num[i];
        if (i != size - 1) std::cout << " ";
        else std::cout << "\n";
    }
}

int main()
{
    setlocale(LC_ALL, "rus");
    int size;
    double result = 0;
    std::cout << "Введите размер массива: ";
    std::cin >> size;
    std::vector<int> numbers(size);
    std::cout << "Введите элементы массива: ";
    for (int i = 0; i < size; i++) std::cin >> numbers[i];
    std::cout << "Введённый массив: ";
    printNumbers(numbers, size);
    int interval;
    std::cout << "Введите временной интервал для остановки и запуска потока: ";
    std::cin >> interval;
    ThreadData* data = new ThreadData{ numbers.data(), size, &result };
    HANDLE hThread = CreateThread(
        NULL,
        0,
        WorkerThread,
        data,
        CREATE_SUSPENDED,
        NULL
    );
    if (hThread == NULL) {
        std::cerr << "Ошибка создания потока\n";
        delete data;
        return 1;
    }
    std::cout << "Поток создан, ждём " << interval << " миллисекунд: \n";
    Sleep(interval);
    std::cout << "Запуск потока" << std::endl;
    ResumeThread(hThread);
    std::cout << "Ждём завершение потока" << std::endl;
    WaitForSingleObject(hThread, INFINITE);
    std::cout << "Поток закончил работу и самоуничтожился" << std::endl;
    CloseHandle(hThread);
    std::cout << "Сумма корней квадратов: " << result << "\n";
    delete data;
    std::cout << "Работа завершена\n";
    return 0;
}