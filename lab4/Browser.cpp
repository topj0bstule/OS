// Browser.cpp
#include <windows.h>
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int N, M;
    cout << "Введите максимальное количество одновременных загрузок (N): ";
    cin >> N;
    cout << "Введите общее количество файлов для скачивания (M > N): ";
    cin >> M;

    if (M <= N) {
        cout << "Ошибка: M должно быть больше N!\n";
        return 1;
    }

    // Создаём объекты синхронизации
    HANDLE hSemaphore = CreateSemaphoreW(nullptr, N, N, L"DownloadSlots");
    HANDLE hMutex = CreateMutexW(nullptr, FALSE, L"LogAccessMutex");
    HANDLE hClosingEvent = CreateEventW(nullptr, TRUE, FALSE, L"BrowserClosingEvent");

    if (!hSemaphore || !hMutex || !hClosingEvent) {
        cout << "Ошибка создания объектов синхронизации!\n";
        return 1;
    }

    vector<PROCESS_INFORMATION> processes;
    processes.reserve(M);

    cout << "Запускаю " << M << " задач загрузки...\n";

    for (int i = 1; i <= M; ++i)
    {
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};

        wchar_t cmdLine[100];
        wsprintfW(cmdLine, L"Downloader.exe %d", i);

        BOOL ok = CreateProcessW(
            nullptr,           // ищем в текущей папке (где лежит Browser.exe)
            cmdLine,
            nullptr, nullptr,
            FALSE, 0,
            nullptr, nullptr,
            &si, &pi
        );

        if (ok)
        {
            CloseHandle(pi.hThread);
            processes.push_back(pi);
        }
        else
        {
            cout << "Не удалось запустить Downloader " << i
                << " (ошибка " << GetLastError() << ")\n";
        }
    }

    cout << "\nBrowser is running. Press Enter to close...\n";
    cin.ignore(10000, '\n');
    cin.get();

    cout << "Browser is closing. Sending termination signal to all downloads...\n";
    SetEvent(hClosingEvent);

    // Ждём завершения всех дочерних процессов
    if (!processes.empty())
    {
        vector<HANDLE> handles;
        for (const auto& p : processes) handles.push_back(p.hProcess);

        WaitForMultipleObjects((DWORD)handles.size(), handles.data(), TRUE, INFINITE);

        for (const auto& p : processes) CloseHandle(p.hProcess);
    }

    CloseHandle(hSemaphore);
    CloseHandle(hMutex);
    CloseHandle(hClosingEvent);

    cout << "Browser closed.\n";
    return 0;
}