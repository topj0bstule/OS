#include "lab2/ProcessManager.hpp"
#include "lab2/Utils.hpp"
#include <vector>
#include <iostream>

namespace lab2 {

bool ProcessInfo::isValid() const {
    return Utils::IsHandleValid(process) && Utils::IsHandleValid(thread);
}

void ProcessInfo::closeAll() {
    Utils::CloseHandleSafe(process);
    Utils::CloseHandleSafe(thread);
}

bool ProcessInfo::wait(DWORD timeout) const {
    return WaitForSingleObject(process, timeout) == WAIT_OBJECT_0;
}

ProcessInfo ProcessManager::CreateChildProcess(
    const std::string& executablePath,
    const std::string& arguments,
    HANDLE hStdInput,
    HANDLE hStdOutput,
    HANDLE hStdError) {
    
    ProcessInfo procInfo;
    
    STARTUPINFOA si;
    Utils::SetupChildStdHandles(hStdInput, hStdOutput, si);
    
    if (Utils::IsHandleValid(hStdError)) {
        si.hStdError = hStdError;
    }
    
    // Создаем командную строку
    std::string commandLine = "\"" + executablePath + "\" " + arguments;
    std::vector<char> cmdLine(commandLine.begin(), commandLine.end());
    cmdLine.push_back('\0');
    
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    
    if (CreateProcessA(
        NULL,                    // Имя модуля (используем командную строку)
        cmdLine.data(),         // Командная строка
        NULL,                    // Атрибуты безопасности процесса
        NULL,                    // Атрибуты безопасности потока
        TRUE,                    // Наследование дескрипторов
        0,                       // Флаги создания
        NULL,                    // Блок окружения
        NULL,                    // Текущий каталог
        &si,                     // STARTUPINFO
        &pi)) {                  // PROCESS_INFORMATION
        
        procInfo.process = pi.hProcess;
        procInfo.thread = pi.hThread;
        procInfo.pid = pi.dwProcessId;
    }
    
    return procInfo;
}

bool ProcessManager::TerminateProcessGracefully(const ProcessInfo& procInfo, DWORD timeout) {
    if (!procInfo.isValid()) {
        return true;
    }
    
    if (procInfo.wait(timeout)) {
        return true;
    }
    
    if (TerminateProcess(procInfo.process, 0)) {
        procInfo.wait(1000);
        return true;
    }
    
    return false;
}

}