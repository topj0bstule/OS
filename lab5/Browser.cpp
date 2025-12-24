#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <mutex>

using namespace std;

// Глобальный мьютекс для синхронизации вывода
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

struct WorkerInfo {
    int id;
    HANDLE hInPipe;
    HANDLE hOutPipe;
    HANDLE hProcess;
    bool is_connected;
};

// Синхронизированный вывод
void Log(const string& message, int worker_id = -1) {
    lock_guard<mutex> lock(g_outputMutex);
    
    if (worker_id >= 1) {
        cout << "[Worker " << worker_id << "] " << message << endl;
    } else {
        cout << "[Browser] " << message << endl;
    }
}

bool writeTaskToWorker(HANDLE hPipe, int task_id, const vector<int>& data, bool is_shutdown = false) {
    TaskHeader header;
    header.task_id = task_id;
    header.data_size = static_cast<int>(data.size());
    header.is_shutdown = is_shutdown;
    
    DWORD written;
    
    // Пишем заголовок
    if (!WriteFile(hPipe, &header, sizeof(TaskHeader), &written, NULL)) {
        DWORD err = GetLastError();
        Log("Ошибка WriteFile заголовка: " + to_string(err));
        return false;
    }
    
    // Пишем данные (если есть)
    if (header.data_size > 0) {
        if (!WriteFile(hPipe, data.data(), header.data_size * sizeof(int), &written, NULL)) {
            DWORD err = GetLastError();
            Log("Ошибка WriteFile данных: " + to_string(err));
            return false;
        }
    }
    
    // Ожидаем подтверждения записи
    DWORD bytes_available = 0;
    if (!PeekNamedPipe(hPipe, NULL, 0, NULL, &bytes_available, NULL)) {
        // Это не критично, продолжаем
    }
    
    return true;
}

bool readResultFromWorker(HANDLE hPipe, int& task_id, int& worker_id, vector<int>& sorted_data) {
    ResultHeader header;
    DWORD read;
    
    // Читаем заголовок
    if (!ReadFile(hPipe, &header, sizeof(ResultHeader), &read, NULL)) {
        DWORD err = GetLastError();
        if (err != ERROR_NO_DATA) {
            Log("Ошибка ReadFile заголовка: " + to_string(err));
        }
        return false;
    }
    
    task_id = header.task_id;
    worker_id = header.worker_id;
    
    // Читаем данные (если есть)
    if (header.data_size > 0) {
        sorted_data.resize(header.data_size);
        if (!ReadFile(hPipe, sorted_data.data(), header.data_size * sizeof(int), &read, NULL)) {
            DWORD err = GetLastError();
            Log("Ошибка ReadFile данных: " + to_string(err));
            return false;
        }
    } else {
        sorted_data.clear();
    }
    
    return true;
}

bool isSorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand((unsigned)time(NULL));
    
    Log("=== БРАУЗЕР (УПРАВЛЯЮЩИЙ ПРОЦЕСС) ===");
    Log("Введите количество Worker-процессов (N): ");
    int N;
    cin >> N;
    Log("Введите общее количество задач (M > N): ");
    int M;
    cin >> M;
    
    if (M <= N) {
        Log("Ошибка: M должно быть больше N!");
        system("pause");
        return 1;
    }
    
    Log("Создание каналов...");
    vector<WorkerInfo> workers(N);
    
    // Создаем все каналы перед запуском Worker
    for (int i = 0; i < N; i++) {
        workers[i].id = i;
        workers[i].is_connected = false;
        
        string inName = "\\\\.\\pipe\\worker_in_" + to_string(i);
        workers[i].hInPipe = CreateNamedPipeA(
            inName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            4096, 4096, 0, NULL
        );
        
        if (workers[i].hInPipe == INVALID_HANDLE_VALUE) {
            Log("Ошибка создания входного канала для worker " + to_string(i));
            return 1;
        }
        
        string outName = "\\\\.\\pipe\\worker_out_" + to_string(i);
        workers[i].hOutPipe = CreateNamedPipeA(
            outName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,
            4096, 4096, 0, NULL
        );
        
        if (workers[i].hOutPipe == INVALID_HANDLE_VALUE) {
            Log("Ошибка создания выходного канала для worker " + to_string(i));
            CloseHandle(workers[i].hInPipe);
            return 1;
        }
        
        Log("Каналы для Worker " + to_string(i) + " созданы");
    }
    
    // Запускаем Worker-ов
    Log("Запуск Worker-процессов...");
    for (int i = 0; i < N; i++) {
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        
        string cmd = "Worker.exe " + to_string(i);
        
        // Запускаем БЕЗ создания новой консоли
        if (!CreateProcessA(
            NULL,
            const_cast<char*>(cmd.c_str()),
            NULL, NULL,
            FALSE,
            0,  // НЕТ CREATE_NEW_CONSOLE!
            NULL, NULL,
            &si, &pi
        )) {
            Log("Ошибка запуска Worker " + to_string(i) + " (код: " + to_string(GetLastError()) + ")");
            return 1;
        }
        
        workers[i].hProcess = pi.hProcess;
        CloseHandle(pi.hThread);
        
        Log("Worker " + to_string(i) + " запущен (PID: " + to_string(pi.dwProcessId) + ")");
        
        // Короткая пауза между запусками
        Sleep(100);
    }
    
    // Даем время Worker-ам инициализироваться
    Sleep(1000);
    
    // Подключаемся к каналам
    Log("Подключение к Worker-ам...");
    for (int i = 0; i < N; i++) {
        Log("Подключение к Worker " + to_string(i) + "...");
        
        // Подключаем входной канал
        if (!ConnectNamedPipe(workers[i].hInPipe, NULL)) {
            DWORD err = GetLastError();
            if (err != ERROR_PIPE_CONNECTED) {
                Log("  Не удалось подключиться к входному каналу: " + to_string(err));
            }
        }
        
        // Подключаем выходной канал
        if (!ConnectNamedPipe(workers[i].hOutPipe, NULL)) {
            DWORD err = GetLastError();
            if (err != ERROR_PIPE_CONNECTED) {
                Log("  Не удалось подключиться к выходному каналу: " + to_string(err));
            }
        }
        
        workers[i].is_connected = true;
        
        // Ждем подтверждения от Worker
        char buffer[100];
        DWORD read;
        DWORD startTime = GetTickCount();
        bool gotResponse = false;
        
        while (GetTickCount() - startTime < 3000) { // 3 секунды таймаут
            if (PeekNamedPipe(workers[i].hOutPipe, NULL, 0, NULL, &read, NULL) && read > 0) {
                if (ReadFile(workers[i].hOutPipe, buffer, sizeof(buffer), &read, NULL)) {
                    buffer[read] = '\0';
                    Log("  Worker " + to_string(i) + " готов: " + string(buffer));
                    gotResponse = true;
                    break;
                }
            }
            Sleep(10);
        }
        
        if (!gotResponse) {
            Log("  Worker " + to_string(i) + " не ответил, но продолжаем...");
        }
    }
    
    Log("Все Worker-ы подключены. Создание задач...");
    
    // Создаем задачи
    vector<vector<int>> tasks(M);
    for (int i = 0; i < M; i++) {
        int size = 1 + rand() % 8; // От 1 до 8 элементов
        for (int j = 0; j < size; j++) {
            tasks[i].push_back(rand() % 100);
        }
        // 10% шанс пустой задачи
        if (rand() % 10 == 0) {
            tasks[i].clear();
        }
    }
    
    Log("Распределение " + to_string(M) + " задач...");
    
    int completed_tasks = 0;
    int current_worker = 0;
    vector<int> tasks_per_worker(N, 0);
    vector<bool> task_success(M, false);
    
    for (int task_id = 0; task_id < M; task_id++) {
        WorkerInfo& worker = workers[current_worker];
        
        Log("Задача " + to_string(task_id) + " -> Worker " + to_string(worker.id) + 
            " (элементов: " + to_string(tasks[task_id].size()) + ")");
        
        // Отправляем задачу
        if (!writeTaskToWorker(worker.hInPipe, task_id, tasks[task_id], false)) {
            Log("  Ошибка отправки задачи " + to_string(task_id));
            current_worker = (current_worker + 1) % N;
            continue;
        }
        
        // Получаем результат
        int result_task_id, result_worker_id;
        vector<int> sorted_data;
        
        if (!readResultFromWorker(worker.hOutPipe, result_task_id, result_worker_id, sorted_data)) {
            Log("  Ошибка получения результата для задачи " + to_string(task_id));
        } else if (result_task_id == task_id) {
            // Проверяем сортировку
            bool is_correct = true;
            for (size_t j = 1; j < sorted_data.size(); j++) {
                if (sorted_data[j] < sorted_data[j-1]) {
                    is_correct = false;
                    break;
                }
            }
            
            if (is_correct || sorted_data.empty()) {
                tasks_per_worker[worker.id]++;
                completed_tasks++;
                task_success[task_id] = true;
                Log("  Успешно обработана Worker " + to_string(result_worker_id));
                
                // Показываем пример для первых 3 задач
                if (task_id < 3 && !tasks[task_id].empty()) {
                    string orig = "    Исходный: [";
                    for (size_t j = 0; j < tasks[task_id].size(); j++) {
                        if (j > 0) orig += " ";
                        orig += to_string(tasks[task_id][j]);
                    }
                    orig += "]";
                    Log(orig);
                    
                    string sorted = "    Отсортированный: [";
                    for (size_t j = 0; j < sorted_data.size(); j++) {
                        if (j > 0) sorted += " ";
                        sorted += to_string(sorted_data[j]);
                    }
                    sorted += "]";
                    Log(sorted);
                }
            } else {
                Log("  Ошибка: массив не отсортирован!");
            }
        } else {
            Log("  Ошибка: неверный ID задачи в ответе (ожидался " + to_string(task_id) + 
                ", получили " + to_string(result_task_id) + ")");
        }
        
        // Переходим к следующему Worker
        current_worker = (current_worker + 1) % N;
    }
    
    Log("Обработка задач завершена.");
    Log("Успешно обработано: " + to_string(completed_tasks) + " из " + to_string(M));
    
    // Отправляем команды завершения
    Log("Отправка команд завершения...");
    for (int i = 0; i < N; i++) {
        vector<int> empty;
        if (writeTaskToWorker(workers[i].hInPipe, -1, empty, true)) {
            Log("Команда завершения отправлена Worker " + to_string(i));
            
            // Ждем подтверждения
            int task_id, worker_id;
            vector<int> dummy;
            readResultFromWorker(workers[i].hOutPipe, task_id, worker_id, dummy);
        }
    }
    
    // Ждем завершения процессов
    Log("Ожидание завершения Worker-процессов...");
    for (int i = 0; i < N; i++) {
        WaitForSingleObject(workers[i].hProcess, 3000);
        DWORD exitCode;
        GetExitCodeProcess(workers[i].hProcess, &exitCode);
        Log("Worker " + to_string(i) + " завершен (код: " + to_string(exitCode) + 
            ", задач обработано: " + to_string(tasks_per_worker[i]) + ")");
        
        CloseHandle(workers[i].hProcess);
        CloseHandle(workers[i].hInPipe);
        CloseHandle(workers[i].hOutPipe);
    }
    
    Log("=== СТАТИСТИКА ===");
    Log("Всего задач: " + to_string(M));
    Log("Успешно обработано: " + to_string(completed_tasks));
    
    if (completed_tasks > 0) {
        Log("Распределение по Worker-ам:");
        for (int i = 0; i < N; i++) {
            Log("  Worker " + to_string(i) + ": " + to_string(tasks_per_worker[i]) + " задач");
        }
    }
    
    Log("Работа браузера завершена. Нажмите Enter...");
    cin.ignore();
    cin.get();
    system("pause");
    return 0;
}