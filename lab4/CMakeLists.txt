cmake_minimum_required(VERSION 3.15)
project(Lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Browser Browser.cpp)
add_executable(Downloader Downloader.cpp)
target_link_libraries(Browser PRIVATE kernel32)
target_link_libraries(Downloader PRIVATE kernel32 user32)
