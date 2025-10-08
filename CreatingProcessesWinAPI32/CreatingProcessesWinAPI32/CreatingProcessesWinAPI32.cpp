#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

void runParentMode();
void runChildMode();

int main(int argc, char* argv[]) {
	if (argc > 1 && strcmp(argv[1], "child") == 0) runChildMode();
	else runParentMode();
	return 0;
}

void runParentMode() {
    int size;
    std::cout << "Enter array size (1-1000): \n";
    std::cin >> size;
    if (size < 1 || size > 1000) {
        std::cout << "Memory allocation failed";
        return;
    }

    std::vector<int> numbers(size);
    std::cout << "Enter " << size << " array numbers:\n";
    for (int i = 0; i < size; i++) {
        std::cin >> numbers[i];
        if (std::cin.fail()) {
            std::cerr << "Invalid input";
            return;
        }
    }

    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };

    HANDLE readPipefromChild, writePipetoChild;
    HANDLE writePipetoParent, readPipefromParent;

    if (!CreatePipe(&readPipefromParent, &writePipetoChild, &sa, 0)) {
        std::cerr << "Pipe creation error (parent to child)\n";
        return;
    }

    if (!CreatePipe(&readPipefromChild, &writePipetoParent, &sa, 0)) {
        std::cerr << "Pipe creation error (child to parent)\n";
        return;
    }

    //Prevent parent-side handles from being inherited
    SetHandleInformation(writePipetoChild, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(readPipefromChild, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = readPipefromParent;     // child reads from here
    si.hStdOutput = writePipetoParent;     // child writes to here
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi{};
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::string cmdLine = "\"" + std::string(exe_path) + "\" child";

    if (!CreateProcessA(NULL, &cmdLine[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "Process creation error\n";
        return;
    }

    DWORD written;
    if (!WriteFile(writePipetoChild, &size, sizeof(size), &written, NULL)) {
        std::cerr << "Failed to write size\n";
        return;
    }

    for (int i = 0; i < size; i++) {
        if (!WriteFile(writePipetoChild, &numbers[i], sizeof(numbers[i]), &written, NULL)) {
            std::cerr << "Failed to write number\n";
            return;
        }
    }

    CloseHandle(writePipetoChild);
    CloseHandle(readPipefromParent);

    int result;
    DWORD read;
    if (ReadFile(readPipefromChild, &result, sizeof(result), &read, NULL)) {
        std::cout << "Minimum value: " << result << "\n";
    }
    else {
        std::cerr << "Failed to read result\n";
    }

    CloseHandle(readPipefromChild);
    CloseHandle(writePipetoParent);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}



void runChildMode() {

    DWORD read;
    int size;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &size, sizeof(size), &read, 0)) {
        std::cerr << "Reading file error";
        return;
    }

    std::vector<int> numbers(size);
    for (int i = 0; i < size; i++) {
        if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE), &numbers[i], sizeof(numbers[i]), &read, 0)) {
            std::cerr << "Reading file error";
            return;
        }
    }

    int min = numbers[0];
    for (int i = 1; i < size; i++) {
        if (min > numbers[i]) min = numbers[i];
    }

    DWORD write;
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), &min, sizeof(min), &write, 0)) {
        std::cerr << "Writing file error file error";
        return;
    }
}