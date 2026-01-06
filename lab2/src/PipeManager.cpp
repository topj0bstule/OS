#include "lab2/PipeManager.hpp"
#include "lab2/Utils.hpp"
#include <iostream>

namespace lab2 {

    bool PipeHandles::isValid() const {
        return Utils::IsHandleValid(read) && Utils::IsHandleValid(write);
    }

    void PipeHandles::closeAll() {
        Utils::CloseHandleSafe(read);
        Utils::CloseHandleSafe(write);
    }

    SECURITY_ATTRIBUTES PipeManager::CreateSecurityAttributes(bool inheritable) {
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = inheritable ? TRUE : FALSE;
        return sa;
    }

    PipeHandles PipeManager::CreatePipe(bool inheritable) {
        PipeHandles handles;
        SECURITY_ATTRIBUTES sa = CreateSecurityAttributes(inheritable);
        
        // Используем CreatePipe с правильными параметрами
        if (!::CreatePipe(&handles.read, &handles.write, &sa, 0)) {
            std::cerr << "Failed to create pipe: " << Utils::GetLastErrorString() << std::endl;
            handles.closeAll();
        }
        
        return handles;
    }

    bool PipeManager::CreatePipePair(PipeHandles& parentToChild, PipeHandles& childToParent) {
        parentToChild = CreatePipe(true);   // Родитель пишет, ребенок читает
        childToParent = CreatePipe(true);   // Ребенок пишет, родитель читает
        
        if (!parentToChild.isValid() || !childToParent.isValid()) {
            parentToChild.closeAll();
            childToParent.closeAll();
            return false;
        }
        
        return true;
    }

} 