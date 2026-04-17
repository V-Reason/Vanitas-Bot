#!/bin/bash

# 遍历当前目录下所有 .cpp 文件
for file in *.cpp; do
    # 如果没有匹配到任何文件，*.cpp 会保持原样，此时应跳过
    [ -e "$file" ] || continue

    # 使用参数扩展去掉 .cpp 后缀，再加上 .txt 后缀
    mv -- "$file" "${file%.cpp}.txt"
done