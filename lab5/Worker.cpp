#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>

using namespace std;

// Глобальный мьютекс для синхронизации вывода (тот же что в Browser)
mutex g_outputMutex;

#pragma pack(push, 1)
struct TaskHeader {
    int task_id;
    int data_size;
    bool is_shutdown;
};

struct ResultHeader {
    int task_id;
    int worker_id;
    int data_size;
};
#pragma pack(pop)

// Синхронизированный вывод
void Log(const string& message, int worker_id) {
    lock_guard<mutex> lock(g_outputMutex);
    
    // Цветной вывод для Worker
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD colors[] = {7, 11, 10, 13, 14, 9, 12}; // Разные цвета для разных Worker
    
    // Выбираем цвет в зависимости от worker_id
    WORD color = colors[worker_id % 7];
    SetConsoleTextAttribute(hConsole, color);
    
    cout << "[Worker " << worker_id << "] " << message << endl;
    
    // Возвращаем обычный цвет
    SetConsoleTextAttribute(hConsole, 7);
}

void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    vector<int> L(n1), R(n2);
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: Worker.exe <worker_id>" << endl;
        return 1;
    }
    
    int worker_id = atoi(argv[1]);
    
    // Даем время Browser создать каналы
    Sleep(500 + worker_id * 100);
    
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    Log("Запущен (PID: " + to_string(GetCurrentProcessId()) + ")", worker_id);
    
    // Формируем имена каналов
    string inPipeName = "\\\\.\\pipe\\worker_in_" + to_string(worker_id);
    string outPipeName = "\\\\.\\pipe\\worker_out_" + to_string(worker_id);
    
    HANDLE hInPipe = INVALID_HANDLE_VALUE;
    HANDLE hOutPipe = INVALID_HANDLE_VALUE;
    
    // Пытаемся подключиться к каналам (с повторными попытками)
    for (int attempt = 0; attempt < 5; attempt++) {
        if (attempt > 0) {
            Log("Попытка " + to_string(attempt + 1) + " подключения...", worker_id);
            Sleep(300);
        }
        
        hInPipe = CreateFileA(
            inPipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0, NULL,
            OPEN_EXISTING,
            0, NULL
        );
        
        if (hInPipe != INVALID_HANDLE_VALUE) {
            hOutPipe = CreateFileA(
                outPipeName.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0, NULL,
                OPEN_EXISTING,
                0, NULL
            );
            
            if (hOutPipe != INVALID_HANDLE_VALUE) {
                Log("Каналы подключены!", worker_id);
                break;
            } else {
                CloseHandle(hInPipe);
                hInPipe = INVALID_HANDLE_VALUE;
            }
        }
    }
    
    if (hInPipe == INVALID_HANDLE_VALUE || hOutPipe == INVALID_HANDLE_VALUE) {
        Log("Не удалось подключиться к каналам", worker_id);
        return 1;
    }
    
    // Отправляем подтверждение Browser
    const char* ready_msg = "READY";
    DWORD written;
    WriteFile(hOutPipe, ready_msg, strlen(ready_msg) + 1, &written, NULL);
    FlushFileBuffers(hOutPipe);
    
    Log("Готов к работе", worker_id);
    
    // Основной рабочий цикл
    while (true) {
        TaskHeader header;
        DWORD bytesRead;
        
        // Читаем заголовок задачи
        if (!ReadFile(hInPipe, &header, sizeof(TaskHeader), &bytesRead, NULL)) {
            DWORD err = GetLastError();
            if (err == ERROR_BROKEN_PIPE || err == ERROR_NO_DATA) {
                Log("Канал закрыт, завершение...", worker_id);
            } else {
                Log("Ошибка чтения заголовка: " + to_string(err), worker_id);
            }
            break;
        }
        
        // Проверяем команду завершения
        if (header.is_shutdown) {
            Log("Получена команда завершения", worker_id);
            
            // Отправляем подтверждение
            ResultHeader resultHeader;
            resultHeader.task_id = -1;
            resultHeader.worker_id = worker_id;
            resultHeader.data_size = 0;
            
            WriteFile(hOutPipe, &resultHeader, sizeof(ResultHeader), &written, NULL);
            FlushFileBuffers(hOutPipe);
            
            break;
        }
        
        Log("Задача " + to_string(header.task_id) + 
            " (элементов: " + to_string(header.data_size) + ")", worker_id);
        
        // Читаем данные задачи
        vector<int> data;
        if (header.data_size > 0) {
            data.resize(header.data_size);
            if (!ReadFile(hInPipe, data.data(), header.data_size * sizeof(int), &bytesRead, NULL)) {
                Log("Ошибка чтения данных", worker_id);
                break;
            }
        }
        
        // Выполняем сортировку (индивидуальный вариант)
        vector<int> sorted_data = data;
        if (!sorted_data.empty()) {
            mergeSort(sorted_data, 0, sorted_data.size() - 1);
        }
        
        // Отправляем результат
        ResultHeader resultHeader;
        resultHeader.task_id = header.task_id;
        resultHeader.worker_id = worker_id;
        resultHeader.data_size = static_cast<int>(sorted_data.size());
        
        if (!WriteFile(hOutPipe, &resultHeader, sizeof(ResultHeader), &written, NULL)) {
            Log("Ошибка отправки результата", worker_id);
            break;
        }
        
        if (resultHeader.data_size > 0) {
            if (!WriteFile(hOutPipe, sorted_data.data(), sorted_data.size() * sizeof(int), &written, NULL)) {
                Log("Ошибка отправки данных", worker_id);
                break;
            }
        }
        
        FlushFileBuffers(hOutPipe);
        Log("Результат отправлен", worker_id);
    }
    
    Log("Завершение работы", worker_id);
    
    CloseHandle(hInPipe);
    CloseHandle(hOutPipe);
    return 0;
}