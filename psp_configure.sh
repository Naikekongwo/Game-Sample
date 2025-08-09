#!/bin/bash

set -e  # 一旦出错就退出脚本
set -o pipefail

# 项目路径
BUILD_DIR=build
OUTPUT_DIR=/mnt/g/PSPGAMEOUTPUT

echo "[1/5] 🔄 清理旧构建目录..."
rm -rf "$BUILD_DIR"
mkdir "$BUILD_DIR"

echo "[2/5] ⚙️ 运行 psp-cmake..."
cd "$BUILD_DIR"
psp-cmake .. -DPSP=ON -DWIN32=OFF

echo "[3/5] 🧱 开始编译..."
make -j$(nproc)  # 使用多核加速编译

echo "[4/5] 🚮 清空输出目录: $OUTPUT_DIR"
rm -rf "$OUTPUT_DIR"/*
mkdir -p "$OUTPUT_DIR"

echo "[5/5] 📦 拷贝构建产物到输出目录"
cp -r ./* "$OUTPUT_DIR"
cp -r ../assets/assets/ "$OUTPUT_DIR/assets/"

echo "✅ 构建并部署完成，输出文件位于: $OUTPUT_DIR"
