#pragma once

#include <windows.h>

namespace lab2 {

    struct PipeHandles {
        HANDLE read = INVALID_HANDLE_VALUE;
        HANDLE write = INVALID_HANDLE_VALUE;
        
        bool isValid() const;
        void closeAll();
    };

    class PipeManager {
        public:
            static PipeHandles CreatePipe(bool inheritable = true);
            static bool CreatePipePair(PipeHandles& parentToChild, PipeHandles& childToParent);
            
        private:
            static SECURITY_ATTRIBUTES CreateSecurityAttributes(bool inheritable);
    };

} // namespace lab2