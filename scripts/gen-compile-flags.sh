#!/usr/bin/env bash
# Generate compile_flags.txt at the repo root by extracting include paths,
# preprocessor defines, and SDK from `xcodebuild -showBuildSettings`. clangd
# (used by Mason / nvim) reads compile_flags.txt to understand the project.
#
# Usage: scripts/gen-compile-flags.sh <path-to.xcodeproj> <project-name>
#
# We use the "Shared Code" target because that's where PluginProcessor.cpp /
# PluginEditor.cpp are compiled, so its settings are the right reference for
# editing those files.

set -euo pipefail

PROJ="${1:?missing path to .xcodeproj}"
NAME="${2:?missing project name}"
TARGET="${NAME} - Shared Code"
OUT="compile_flags.txt"

settings=$(xcodebuild -project "$PROJ" -target "$TARGET" -configuration Debug -showBuildSettings 2>/dev/null)

extract() { sed -n "s/^ *${1} = //p" <<<"$settings"; }

hdrs=$(extract HEADER_SEARCH_PATHS)
defs=$(extract GCC_PREPROCESSOR_DEFINITIONS)
sdk=$(extract SDKROOT)
std=$(extract CLANG_CXX_LANGUAGE_STANDARD)
lib=$(extract CLANG_CXX_LIBRARY)

{
  echo "-xc++"
  echo "-std=${std:-c++17}"
  [[ -n "$lib" ]] && echo "-stdlib=${lib}"
  if [[ -n "$sdk" ]]; then
    echo "-isysroot"
    echo "$sdk"
  fi
  # eval used so shell-quoted tokens (paths or defines containing spaces) are
  # split correctly into array elements, matching how xcodebuild emits them.
  eval "h=($hdrs)"
  for x in "${h[@]}"; do echo "-I$x"; done
  eval "d=($defs)"
  for x in "${d[@]}"; do echo "-D$x"; done
} > "$OUT"

echo "Wrote $OUT ($(wc -l < "$OUT" | tr -d ' ') flags)."
