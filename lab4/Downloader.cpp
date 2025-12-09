// Downloader.cpp — ФИНАЛЬНАЯ ВЕРСИЯ С ПРЕРЫВАНИЕМ АКТИВНЫХ ЗАГРУЗОК
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

bool isPrime(int n) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0) return false;
    return true;
}

int countPrimes() {
    int cnt = 0;
    for (int i = 2; i <= 10000; ++i)
        if (isPrime(i)) ++cnt;
    return cnt;
}

int wmain(int argc, wchar_t* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    srand((unsigned)time(nullptr) ^ GetCurrentProcessId());

    int fileId = (argc > 1) ? _wtoi(argv[1]) : 1;
    wchar_t fileName[64];
    wsprintfW(fileName, L"file_%03d.dat", fileId);

    // Открываем объекты
    HANDLE hSem = OpenSemaphoreW(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, L"DownloadSlots");
    HANDLE hMutex = OpenMutexW(SYNCHRONIZE | MUTEX_MODIFY_STATE, FALSE, L"LogAccessMutex");
    HANDLE hEvent = OpenEventW(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, L"BrowserClosingEvent");

    if (!hSem || !hMutex || !hEvent) {
        wcout << L"[PID: " << GetCurrentProcessId() << L"] Ошибка подключения к браузеру!\n";
        return 1;
    }

    DWORD pid = GetCurrentProcessId();
    bool slotTaken = false;

    HANDLE waits[2] = { hSem, hEvent };

    DWORD result = WaitForMultipleObjects(2, waits, FALSE, INFINITE);

    if (result == WAIT_OBJECT_0 + 1) {
        // Браузер закрывается (до взятия слота)
        WaitForSingleObject(hMutex, INFINITE);
        wcout << L"[PID: " << pid << L"] Download of '" << fileName << L"' interrupted by browser closing.\n";
        ReleaseMutex(hMutex);
        goto end;
    }

    slotTaken = true;

    // === НАЧАЛО ЗАГРУЗКИ ===
    WaitForSingleObject(hMutex, INFINITE);
    wcout << L"[PID: " << pid << L"] Connection established. Starting download of '" << fileName << L"'...\n";
    ReleaseMutex(hMutex);

    // Имитация скачивания с проверкой события закрытия (разбиваем на мелкие интервалы)
    int totalSleep = 1000 + rand() % 2001;  // 1–3 сек
    int interval = 100;  // проверяем каждые 100 мс
    bool interrupted = false;
    while (totalSleep > 0 && !interrupted) {
        DWORD waitRes = WaitForSingleObject(hEvent, min(interval, (unsigned)totalSleep));
        if (waitRes == WAIT_OBJECT_0) {
            // Сигнал закрытия во время "загрузки"
            interrupted = true;
        }
        else {
            totalSleep -= interval;
        }
    }

    if (interrupted) {
        // Прервано во время обработки — выводим interrupted и НЕ освобождаем слот
        WaitForSingleObject(hMutex, INFINITE);
        wcout << L"[PID: " << pid << L"] Download of '" << fileName << L"' interrupted by browser closing.\n";
        ReleaseMutex(hMutex);
        goto end;
    }

    // Вариант 9 (если не прервано)
    int primes = countPrimes();

    // === УСПЕШНОЕ ЗАВЕРШЕНИЕ ===
    WaitForSingleObject(hMutex, INFINITE);
    wcout << L"[PID: " << pid << L"] File '" << fileName << L"' processed successfully. "
        << L"Found " << primes << L" prime numbers (1..10000).\n";
    ReleaseMutex(hMutex);

    // Освобождаем слот только при успешном завершении
    ReleaseSemaphore(hSem, 1, nullptr);

end:
    CloseHandle(hSem);
    CloseHandle(hMutex);
    CloseHandle(hEvent);
    return 0;
}