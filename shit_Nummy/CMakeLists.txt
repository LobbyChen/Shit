cmake_minimum_required(VERSION 3.10)
project(shit)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 跨平台线程库
find_package(Threads REQUIRED)

add_executable(shit main.cpp)

add_executable(inequality inequality.cpp)
target_include_directories(inequality PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(inequality PRIVATE Threads::Threads)

add_executable(product product.cpp)
target_include_directories(product PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(product PRIVATE Threads::Threads)
