#!/bin/bash

cd "$(dirname "$0")" || exit 1

export LD_LIBRARY_PATH="$(pwd)/lib"
echo "Looking for LLVM library in $LD_LIBRARY_PATH"

#find "$(pwd)"/../{include,src}/ \( -name "*.cpp" -o -name "*.hpp" \) -exec \
#  ./hyde "{}" \
#         -hyde-update                      \
#         -p "../cmake-build-debug" \
#         --                                 \
#         -std=gnu++2b \
#         -I/usr/lib/llvm13/lib/clang/13.0.1/include \
#         -I/usr/include/freetype2 \;
#

rm -rvf "$(pwd)/../docs/src/*"

rsync -rtvz "$(pwd)/../src" "$(pwd)/../docs/src/"
rsync -rtvz "$(pwd)/../include" "$(pwd)/../docs/src/"

find "$(pwd)"/../docs/src/{include,src}/ \( -name "*.cpp" -o -name "*.hpp" \) -exec \
  mv "{}" "{}.md" \;

#find "$(pwd)"/../docs/_data/src/{include,src}/ \( -name "*.yml" \) -exec \
#  sh -c 'echo -e "source: >-\n$(cat {})" > {}.new && mv -f {}.new {}' \;
