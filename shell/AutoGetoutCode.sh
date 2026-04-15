#!/bin/bash

# 用法：./collect_cpp_h.sh [源目录] [目标目录]
SOURCE_DIR="${1:-.}"
DEST_DIR="${2:-./collected_files}"

# 检查源目录是否存在
if [ ! -d "$SOURCE_DIR" ]; then
    echo "错误：源目录 '$SOURCE_DIR' 不存在。"
    exit 1
fi

# 创建目标目录（如果不存在）
mkdir -p "$DEST_DIR" || { echo "无法创建目标目录 '$DEST_DIR'"; exit 1; }

echo "正在从 '$SOURCE_DIR' 查找 .cpp 和 .h 文件..."
echo "目标文件夹: '$DEST_DIR'"

# 查找所有 .cpp 和 .h 文件，并逐个复制
find "$SOURCE_DIR" -type f \( -name "*.cpp" -o -name "*.h" \) -print0 | while IFS= read -r -d '' file; do
    filename=$(basename "$file")
    dest_file="$DEST_DIR/$filename"

    # 若文件已存在则覆盖（显示警告），如需避免覆盖改为：cp -n "$file" "$dest_file"
    if [ -e "$dest_file" ]; then
        echo "警告：覆盖已有文件 '$filename'"
    fi

    if cp "$file" "$dest_file"; then
        echo "已复制: $file -> $dest_file"
    else
        echo "复制失败: $file"
    fi
done

echo "操作完成。所有文件已放入 '$DEST_DIR'"
