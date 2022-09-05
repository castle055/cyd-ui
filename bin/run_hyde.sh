#!/bin/bash

cd "$(dirname "$0")" || exit 1

export LD_LIBRARY_PATH="$(pwd)/lib"
echo "Looking for LLVM library in $LD_LIBRARY_PATH"

./hyde $(find "$(pwd)"/../{include,src}/ -name "*.cpp" -o -name "*.h")  \
       -hyde-update                      \
       -p "../cmake-build-debug" \
       --                                 \
       -std=gnu++2b \
       -I/usr/lib/llvm13/lib/clang/13.0.1/include \
       -I/usr/include/freetype2

