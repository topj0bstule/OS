add_library(thread_code CreatingThreadWinAPI32.cpp)
target_include_directories(thread_code PUBLIC ${CMAKE_SOURCE_DIR}/include)
