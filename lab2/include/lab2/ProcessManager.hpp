#pragma once

#include <windows.h>
#include <string>

namespace lab2 {

    struct ProcessInfo {
        HANDLE process = INVALID_HANDLE_VALUE;
        HANDLE thread = INVALID_HANDLE_VALUE;
        DWORD pid = 0;
        
        bool isValid() const;
        void closeAll();
        bool wait(DWORD timeout = INFINITE) const;
    };

    class ProcessManager {
        public:
            static ProcessInfo CreateChildProcess(
                const std::string& executablePath,
                const std::string& arguments,
                HANDLE hStdInput,
                HANDLE hStdOutput,
                HANDLE hStdError = GetStdHandle(STD_ERROR_HANDLE));
            
            static bool TerminateProcessGracefully(const ProcessInfo& procInfo, DWORD timeout = 5000);
    };

} // namespace lab2