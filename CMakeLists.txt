cmake_minimum_required(VERSION 3.14)
project(MySTL)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试
enable_testing()

# 添加 Google Test
add_subdirectory(third_party/googletest)

# 添加头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/third_party/googletest/googletest/include
)

# 添加测试目录
add_subdirectory(tests) 