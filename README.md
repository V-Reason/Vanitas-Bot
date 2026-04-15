# Vanitas Bot
> Vanitas Vanitatum Et Omnia Vanitas.

## 注意
- 项目必须使用 **C++20** 及以上标准

## 项目结构
> 严格遵循，方便打包
```txt
Vanitas Bot/
├── code/                   # 所有的源代码
├── build/                  # 构建产物
├── log/                    # 日志文件（.log)
├── test_cases/             # 本地测试用的IO文件
├── .clang-format
├── .editorconfig
├── .gitattributes
├── .gitignore
├── LICENSE
└── README.md
```

## VSCode插件
> 必须配置，规范代码
-  **C/C++ Extension Pack**
-  **EditorConfig for VS Code**
-  **CMake Tools**
-  **Clang-Format**

## Tips
> 参考配置文件

> `launch.json`
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "T:\\mingw64\\bin\\gdb.exe",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "将反汇编风格设置为 Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}

```
> `CMakeLists.txt`
```txt
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
    # Release 模式：O3极限优化，开启 CPU 原生指令集加速位运算，关闭调试信息
    target_compile_options(Vanitas-Bot PRIVATE -O3 -march=native -Wall -DNDEBUG)
endif()
```
