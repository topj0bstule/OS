#include "lab2/ArrayProcessor.hpp"
#include "lab2/PipeManager.hpp"
#include "lab2/ProcessManager.hpp"
#include "lab2/Utils.hpp"
#include <iostream>
#include <string>

void RunChildProcess() {
    int size = 0;
    
    // Чтение размера массива
    if (!lab2::ArrayProcessor::ReadArraySizeFromPipe(
            GetStdHandle(STD_INPUT_HANDLE), size)) {
        std::cerr << "Child: Error reading array size" << std::endl;
        return;
    }
    
    if (size <= 0) {
        std::cerr << "Child: Invalid array size: " << size << std::endl;
        return;
    }
    
    // Чтение массива
    std::vector<int> arr;
    if (!lab2::ArrayProcessor::ReadArrayFromPipe(
            GetStdHandle(STD_INPUT_HANDLE), arr, size)) {
        std::cerr << "Child: Error reading array" << std::endl;
        return;
    }
    
    // Вычисление среднего
    double average = lab2::ArrayProcessor::CalculateAverage(arr);
    
    // Отправка результата
    if (!lab2::ArrayProcessor::WriteDoubleToPipe(
            GetStdHandle(STD_OUTPUT_HANDLE), average)) {
        std::cerr << "Child: Error writing result" << std::endl;
    }
}

void RunParentProcess() {
    // Ввод размера массива
    int size = 0;
    std::cout << "Enter array size: ";
    std::cin >> size;
    
    if (size <= 0) {
        std::cout << "Invalid array size" << std::endl;
        return;
    }
    
    // Генерация и вывод массива
    std::vector<int> arr = lab2::ArrayProcessor::GenerateRandomArray(size);
    lab2::ArrayProcessor::PrintArray(arr);
    
    // Создание каналов
    lab2::PipeHandles parentToChild, childToParent;
    if (!lab2::PipeManager::CreatePipePair(parentToChild, childToParent)) {
        std::cerr << "Failed to create pipes" << std::endl;
        return;
    }
    
    // Создание дочернего процесса
    lab2::ProcessInfo childProcess = lab2::ProcessManager::CreateChildProcess(
        "lab2.exe", "child", parentToChild.read, childToParent.write);
    
    if (!childProcess.isValid()) {
        std::cerr << "Failed to create child process" << std::endl;
        parentToChild.closeAll();
        childToParent.closeAll();
        return;
    }
    
    // Закрываем ненужные родителю концы каналов
    lab2::Utils::CloseHandleSafe(parentToChild.read);
    lab2::Utils::CloseHandleSafe(childToParent.write);
    
    // Отправка данных дочернему процессу
    bool success = true;
    
    if (!lab2::ArrayProcessor::WriteArraySizeToPipe(parentToChild.write, size)) {
        std::cerr << "Error writing array size to pipe" << std::endl;
        success = false;
    }
    
    if (success && !lab2::ArrayProcessor::WriteArrayToPipe(parentToChild.write, arr)) {
        std::cerr << "Error writing array to pipe" << std::endl;
        success = false;
    }
    
    lab2::Utils::CloseHandleSafe(parentToChild.write);
    
    // Чтение результата
    if (success) {
        double result = 0.0;
        if (lab2::ArrayProcessor::ReadDoubleFromPipe(childToParent.read, result)) {
            std::cout << "Average of array elements: " << result << std::endl;
        } else {
            std::cerr << "Error reading result from child process" << std::endl;
        }
    }
    
    lab2::Utils::CloseHandleSafe(childToParent.read);
    
    // Ожидание завершения дочернего процесса
    childProcess.wait();
    childProcess.closeAll();
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "child") {
        RunChildProcess();
    } else {
        RunParentProcess();
    }
    system("pause");
    return 0;
}