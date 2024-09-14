#!/bin/bash

script_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
repo_root="${script_root%/tools}"
build_root="${repo_root}/build"
compile_commands="${build_root}/compile_commands.json"

if [ ! -f "${compile_commands}" ]; then
    echo "Could not find a 'compile_commands.json' file. Please build the project first."
    exit 1
fi

cxx_files_list="$(cd "${repo_root}" && git ls-tree --full-tree --name-only -r HEAD | grep -E ".*(\.cpp|\.hpp)")"

(cd "${repo_root}" && clang-tidy -config-file="${repo_root}/.clang-tidy" $cxx_files_list -p "${build_root}")
