#!/bin/sh
find . -type f -not -path "*amalgamation/*" \( -name "*.h" -or -name "*.cpp" \) -exec clang-format -i {} \;
