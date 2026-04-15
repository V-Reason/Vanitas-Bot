cmake_minimum_required(VERSION 3.10)

# 项目名称
project(Vanitas-Bot)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译输出路径（自动把程序放到 build 文件夹）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 自动加载 code/ 文件夹下所有源码
file(GLOB_RECURSE SOURCE_FILES
    "code/*.cpp"
    "code/*.h"
)

# 生成可执行程序
add_executable(Vanitas-Bot ${SOURCE_FILES})

# 指定头文件包含路径 (Modern CMake 写法)
target_include_directories(Vanitas-Bot PRIVATE code)

# 编译优化（Debug调试 / Release比赛用）
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Debug 模式：开启所有警告
    target_compile_options(Vanitas-Bot PRIVATE -g -Wall -Wextra)
else()
    # Release 模式：O2极限优化，开启 CPU 原生指令集加速位运算，关闭调试信息
    target_compile_options(Vanitas-Bot PRIVATE -O2 -march=native -Wall -DNDEBUG)
endif()
