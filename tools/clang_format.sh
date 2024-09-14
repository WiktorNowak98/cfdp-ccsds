#!/bin/bash

script_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
repo_root="${script_root%/tools}"

cxx_files_list="$(cd "${repo_root}" && git ls-tree --full-tree --name-only -r HEAD | grep -E ".*(\.cpp|\.hpp)")"

(cd "${repo_root}" && clang-format --verbose -i $cxx_files_list)
