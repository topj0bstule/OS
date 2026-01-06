#include "lab2/Utils.hpp"
#include <sstream>

namespace lab2 {

    void Utils::CloseHandleSafe(HANDLE& handle) {
        if (IsHandleValid(handle)) {
            CloseHandle(handle);
            handle = INVALID_HANDLE_VALUE;
        }
    }

    bool Utils::IsHandleValid(HANDLE handle) {
        return handle != INVALID_HANDLE_VALUE && handle != NULL;
    }

    std::string Utils::GetLastErrorString() {
        DWORD error = GetLastError();
        if (error == 0) {
            return "No error";
        }
        
        LPSTR messageBuffer = nullptr;
        DWORD size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&messageBuffer,
            0,
            NULL);
        
        std::string message(messageBuffer, size);
        LocalFree(messageBuffer);
        
        return message;
    }

    void Utils::SetupChildStdHandles(HANDLE hStdInput, HANDLE hStdOutput, STARTUPINFOA& si) {
        ZeroMemory(&si, sizeof(STARTUPINFOA));
        si.cb = sizeof(STARTUPINFOA);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = hStdInput;
        si.hStdOutput = hStdOutput;
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }

}