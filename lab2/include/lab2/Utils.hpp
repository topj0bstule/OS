#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace lab2 {

    class Utils {
    public:
        static void CloseHandleSafe(HANDLE& handle);
        static bool IsHandleValid(HANDLE handle);
        static std::string GetLastErrorString();
        static void SetupChildStdHandles(HANDLE hStdInput, HANDLE hStdOutput, STARTUPINFOA& si);
    };

} 