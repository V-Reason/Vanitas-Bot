#!/usr/bin/env python3
"""
C++ 文件一键合并脚本
自动扫描 code/ 下所有 .cpp，解析本地 #include，输出 combined.cpp
"""

import os
import re
import sys
from pathlib import Path
from typing import List, Set, Optional


class CppCombiner:
    def __init__(self, base_dir: str = "code", output_file: str = "combined.cpp"):
        self.base_dir = Path(base_dir).resolve()
        self.output_file = Path(output_file)
        self.included_headers: Set[str] = set()
        self.header_content: List[str] = []
        self.source_bodies: List[tuple] = []  # (file_path, content_lines)

        if not self.base_dir.is_dir():
            print(
                f"错误：文件夹 '{self.base_dir}' 不存在！请将脚本放在包含 'code' 文件夹的目录下运行。"
            )
            sys.exit(1)

    def find_header(self, header_name: str, current_file_dir: Path) -> Optional[Path]:
        """在 code 目录及子目录中递归查找头文件"""
        # 1. 先在与源文件同目录下查找
        candidate = current_file_dir / header_name
        if candidate.is_file():
            return candidate.resolve()

        # 2. 在 code 目录及其所有子目录中搜索
        for root, dirs, files in os.walk(self.base_dir):
            root_path = Path(root)
            candidate = root_path / header_name
            if candidate.is_file():
                return candidate.resolve()

        return None

    def resolve_includes(
        self, file_path: Path, processing_headers: Set[str] = None
    ) -> List[str]:
        if processing_headers is None:
            processing_headers = set()

        abs_path = file_path.resolve()
        if str(abs_path) in processing_headers:
            return []  # 防止循环包含

        processing_headers.add(str(abs_path))

        try:
            with open(abs_path, "r", encoding="utf-8", errors="replace") as f:
                lines = f.readlines()
        except Exception as e:
            print(f"警告：无法读取文件 {abs_path}: {e}", file=sys.stderr)
            return []

        output_lines = []
        include_re = re.compile(r'^\s*#\s*include\s*"([^"]+)"')
        pragma_once_re = re.compile(r"^\s*#\s*pragma\s+once\s*(//.*)?$")

        for line in lines:
            m = include_re.match(line)
            if m:
                header_name = m.group(1)
                header_path = self.find_header(header_name, abs_path.parent)
                if header_path:
                    if str(header_path) not in self.included_headers:
                        self.included_headers.add(str(header_path))
                        # 递归处理头文件内部的 #include
                        header_lines = self.resolve_includes(
                            header_path, processing_headers.copy()
                        )
                        self.header_content.append(
                            f"// BEGIN included header: {header_path}\n"
                        )
                        self.header_content.extend(header_lines)
                        self.header_content.append(
                            f"// END included header: {header_path}\n\n"
                        )
                    # 跳过该 #include 行（已合并内容）
                else:
                    # 未找到头文件，保留原 #include 行并给出警告
                    print(
                        f"警告：未找到头文件 '{header_name}'，引用自 {abs_path}",
                        file=sys.stderr,
                    )
                    output_lines.append(line)
            else:
                # 跳过 #pragma once
                if pragma_once_re.match(line):
                    continue
                output_lines.append(line)

        return output_lines

    def collect_source_files(self):
        """自动扫描 code 目录下所有 .cpp 文件"""
        print(f"正在扫描 {self.base_dir} 下的所有 .cpp 文件...")
        cpp_files = list(self.base_dir.rglob("*.cpp"))
        if not cpp_files:
            print(f"错误：在 '{self.base_dir}' 中没有找到任何 .cpp 文件！")
            sys.exit(1)

        print(f"找到 {len(cpp_files)} 个源文件：")
        for f in cpp_files:
            print(f"  - {f.relative_to(self.base_dir.parent)}")
        print()

        for cpp_file in cpp_files:
            print(f"处理中：{cpp_file}")
            body_lines = self.resolve_includes(cpp_file)
            self.source_bodies.append((str(cpp_file), body_lines))

    def generate_output(self):
        """生成合并后的文件"""
        with open(self.output_file, "w", encoding="utf-8") as f:
            f.write("// ==========================================\n")
            f.write("// 自动合并的 C++ 源文件\n")
            f.write(f"// 生成时间：{self._get_current_time()}\n")
            f.write("// ==========================================\n\n")

            f.write("// ========== 合并的头文件 ==========\n\n")
            for line in self.header_content:
                f.write(line)
            f.write("// ========== 头文件结束 ==========\n\n")

            for src_path, body_lines in self.source_bodies:
                f.write(f"// ========== 源文件：{src_path} ==========\n\n")
                f.writelines(body_lines)
                f.write("\n\n")

        print(f"\n✅ 成功！合并后的文件已保存为：{self.output_file}")

    def _get_current_time(self):
        from datetime import datetime

        return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def main():
    # 可以手动修改这里的默认值
    DEFAULT_CODE_DIR = "code"
    DEFAULT_OUTPUT = "Vanitas-Bot.cpp"

    combiner = CppCombiner(base_dir=DEFAULT_CODE_DIR, output_file=DEFAULT_OUTPUT)
    combiner.collect_source_files()
    combiner.generate_output()


if __name__ == "__main__":
    main()
