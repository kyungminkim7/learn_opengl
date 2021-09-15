cmake_minimum_required(VERSION 3.5...3.10)
project(l1_hello_window)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} lgl::lgl)
