cmake_minimum_required(VERSION 3.10)
project(Lab2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение директорий
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Основной исполняемый файл
add_executable(lab2 
    src/main.cpp
    src/ArrayProcessor.cpp
    src/PipeManager.cpp
    src/ProcessManager.cpp
    src/Utils.cpp
)

# Настройки для Windows
if(WIN32)
    target_link_libraries(lab2 Kernel32.lib)
    set_target_properties(lab2 PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

# Установка выходного каталога (опционально)
set_target_properties(lab2 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)