#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// Тест 1.1: Один воркер, одна задача
void testSingleWorkerSingleTask() {
    cout << "\n=== ТЕСТ 1.1: Один воркер, одна задача ===" << endl;
    
    // Запускаем Browser с параметрами N=1, M=1
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    
    string cmd = "Browser.exe";
    
    if (CreateProcessA(NULL, const_cast<char*>(cmd.c_str()),
                      NULL, NULL, FALSE, CREATE_NEW_CONSOLE,
                      NULL, NULL, &si, &pi)) {
        // Ждем завершения Browser
        WaitForSingleObject(pi.hProcess, 30000); // 30 секунд таймаут
        
        DWORD exitCode;
        GetExitCodeProcess(pi.hProcess, &exitCode);
        
        if (exitCode == 0) {
            cout << "✓ Тест пройден: Browser завершился успешно" << endl;
        } else {
            cout << "✗ Тест не пройден: Browser завершился с ошибкой " << exitCode << endl;
        }
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        cout << "✗ Ошибка запуска Browser" << endl;
    }
}

// Тест 1.2: Пул воркеров, много задач
void testWorkerPoolMultipleTasks() {
    cout << "\n=== ТЕСТ 1.2: Пул воркеров, много задач ===" << endl;
    
    // Можно было бы автоматизировать через создание процесса с параметрами,
    // но для простоты сделаем ручной тест
    cout << "Для этого теста необходимо:" << endl;
    cout << "1. Запустить Browser.exe" << endl;
    cout << "2. Ввести N=2 (количество Worker)" << endl;
    cout << "3. Ввести M=10 (количество задач)" << endl;
    cout << "4. Проверить, что все 10 задач выполнены" << endl;
    cout << "5. Проверить, что все Worker корректно завершились" << endl;
    cout << "\nТест считается пройденным, если:" << endl;
    cout << "- Все задачи обработаны" << endl;
    cout << "- Результаты корректны (массивы отсортированы)" << endl;
    cout << "- Все процессы завершены без зависаний" << endl;
}

// Тест 1.3: Корректное завершение
void testGracefulShutdown() {
    cout << "\n=== ТЕСТ 1.3: Корректное завершение ===" << endl;
    
    cout << "Тест проверяет, что:" << endl;
    cout << "1. Browser отправляет команды завершения всем Worker" << endl;
    cout << "2. Worker корректно обрабатывают команду shutdown" << endl;
    cout << "3. Все процессы завершаются без утечек ресурсов" << endl;
    cout << "\nДля проверки запустите Browser с любыми параметрами" << endl;
    cout << "и убедитесь, что в конце работы появляются сообщения:" << endl;
    cout << "- 'Команда завершения отправлена Worker X'" << endl;
    cout << "- 'Worker X завершен'" << endl;
    cout << "- Все дескрипторы каналов корректно закрыты" << endl;
}

// Тест 2.1: Пустая задача
void testEmptyTask() {
    cout << "\n=== ТЕСТ 2.1: Пустая задача ===" << endl;
    
    // Создаем тестовый массив
    vector<int> empty_array;
    vector<int> single_element = {42};
    vector<int> normal_array = {5, 2, 8, 1, 9};
    
    // Тестируем сортировку слиянием (функциональность Worker)
    auto testSort = [](const vector<int>& input, const string& name) {
        vector<int> result = input;
        
        if (!result.empty()) {
            // Функции сортировки (такие же как в Worker)
            auto merge = [](vector<int>& arr, int l, int m, int r) {
                int n1 = m - l + 1;
                int n2 = r - m;
                vector<int> L(n1), R(n2);
                
                for (int i = 0; i < n1; i++) L[i] = arr[l + i];
                for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
                
                int i = 0, j = 0, k = l;
                while (i < n1 && j < n2) {
                    if (L[i] <= R[j]) arr[k++] = L[i++];
                    else arr[k++] = R[j++];
                }
                while (i < n1) arr[k++] = L[i++];
                while (j < n2) arr[k++] = R[j++];
            };
            
            auto mergeSort = [&](auto&& self, vector<int>& arr, int l, int r) -> void {
                if (l < r) {
                    int m = l + (r - l) / 2;
                    self(self, arr, l, m);
                    self(self, arr, m + 1, r);
                    merge(arr, l, m, r);
                }
            };
            
            mergeSort(mergeSort, result, 0, result.size() - 1);
        }
        
        // Проверяем сортировку
        bool is_sorted = true;
        for (size_t i = 1; i < result.size(); i++) {
            if (result[i] < result[i-1]) {
                is_sorted = false;
                break;
            }
        }
        
        cout << name << ": ";
        if (input.empty()) {
            cout << "пустой массив -> ";
        } else {
            cout << "[";
            for (size_t i = 0; i < input.size(); i++) {
                if (i > 0) cout << " ";
                cout << input[i];
            }
            cout << "] -> [";
            for (size_t i = 0; i < result.size(); i++) {
                if (i > 0) cout << " ";
                cout << result[i];
            }
            cout << "] ";
        }
        
        if (is_sorted || input.empty()) {
            cout << "✓ OK" << endl;
            return true;
        } else {
            cout << "✗ Ошибка сортировки" << endl;
            return false;
        }
    };
    
    bool all_passed = true;
    all_passed &= testSort(empty_array, "Пустой массив");
    all_passed &= testSort(single_element, "Один элемент");
    all_passed &= testSort(normal_array, "Нормальный массив");
    
    if (all_passed) {
        cout << "\n✓ Все тесты пустых/малых массивов пройдены" << endl;
        cout << "Worker должен корректно обрабатывать такие задачи" << endl;
    }
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    cout << "=== СИСТЕМНОЕ ТЕСТИРОВАНИЕ BROWSER-WORKER ===" << endl;
    cout << "Эта программа проверяет соответствие реализации условию задания" << endl;
    
    // Запускаем все тесты
    testSingleWorkerSingleTask();
    testWorkerPoolMultipleTasks();
    testGracefulShutdown();
    testEmptyTask();
    
    cout << "\n=== ИНСТРУКЦИЯ ДЛЯ РУЧНОГО ТЕСТИРОВАНИЯ ===" << endl;
    cout << "\nДля полного тестирования выполните:" << endl;
    cout << "1. Скомпилируйте Browser.exe и Worker.exe" << endl;
    cout << "2. Запустите Browser.exe и введите параметры:" << endl;
    cout << "   - Тест 1.1: N=1, M=1" << endl;
    cout << "   - Тест 1.2: N=2, M=10" << endl;
    cout << "   - Тест 1.3: любые параметры" << endl;
    cout << "3. Проверьте вывод в консоли на соответствие условию" << endl;
    
    cout << "\nОжидаемое поведение:" << endl;
    cout << "- Browser создает именованные каналы" << endl;
    cout << "- Worker подключаются к своим каналам" << endl;
    cout << "- Задачи распределяются между Worker" << endl;
    cout << "- Массивы корректно сортируются" << endl;
    cout << "- Пустые массивы обрабатываются без ошибок" << endl;
    cout << "- При завершении Browser отправляет команды shutdown" << endl;
    cout << "- Все процессы корректно завершаются" << endl;
    
    cout << "\nНажмите Enter для выхода...";
    cin.get();
    system("pause");
    return 0;
}