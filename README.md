# Vanitas Bot
> Vanitas Vanitatum Et Omnia Vanitas.

## 项目结构
> 严格遵循，方便打包
```txt
Vanitas Bot/
├── code/                   # 所有的源代码
├── build/                  # 构建产物
├── test_cases/             # 本地测试用的数据文件
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
-  **Clang-Format**

## Tips
> 参考配置文件
```json
{
    // launch.json
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/Vanitas-Bot.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "T:\\mingw64\\bin\\gdb.exe",
            "preLaunchTask": "C/C++: g++.exe 生成项目",
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
```json
{
    // tasks.json
    "tasks": [
        {
            "type": "cppbuild",
            "label": "C/C++: g++.exe 生成项目",
            "command": "T:\\mingw64\\bin\\g++.exe",
            "args": [
                "-fdiagnostics-color=always",
                "-g",
                "${workspaceFolder}\\code\\*.cpp",
                "-o",
                "${workspaceFolder}\\build\\Vanitas-Bot.exe"
            ],
            "options": {
                "cwd": "${workspaceFolder}"
            },
            "problemMatcher": [
                "$gcc"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "detail": "调试器生成的任务。"
        }
    ],
    "version": "2.0.0"
}

```
