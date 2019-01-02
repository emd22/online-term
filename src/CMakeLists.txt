cmake_minimum_required(VERSION 3.5)

file (GLOB_RECURSE SOURCES "*.c")
file (GLOB_RECURSE HEADERS "../include/lib/*.h")

set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall")
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/lib
)
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
  
add_executable(lib ${SOURCES} ${HEADERS})
target_link_libraries(lib Threads::Threads)