#!/bin/bash

script_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
repo_root="${script_root%/tools}"
build_dir="${repo_root}/build"

clean_run=false
compile_tests=false
shared_lib_option="OFF"
cxx_compiler="g++"

while [[ $# -gt 0 ]]; do
	case $1 in
	--clang)
		cxx_compiler="clang++"
		shift
		;;
	--no-cache)
		clean_run=true
		shift
		;;
	--compile-tests)
		compile_tests=true
		shift
		;;
	--shared-lib)
		shared_lib_option="ON"
		shift
		;;
	*)
		echo "Unknown option $1"
		exit 1
		;;
	esac
done

echo
echo "Build configuration:"
echo "Repository Root     = ${repo_root}"
echo "Build Dir           = ${build_dir}"
echo "C++ Compiler        = ${cxx_compiler}"
echo "Clean Run           = ${clean_run}"
echo "Compile Tests       = ${compile_tests}"
echo "Compile Shared Lib  = ${shared_lib_option}"
echo

cmake -G Ninja -D CMAKE_CXX_COMPILER="${cxx_compiler}" -D BUILD_SHARED_LIBS="${shared_lib_option}" -D COMPILE_TESTS="${compile_tests}" -S "${repo_root}" -B build

if [ ! -f "${build_dir}/CMakeCache.txt" ]; then
	echo -e "\nBuild directory or CMakeCache.txt does not exist."
	echo "Please run the script with --configure option first."
	exit 1
fi

if [ "${clean_run}" = true ]; then
	echo -e "\nClearing the cache for fresh build...\n"
	(cd "${repo_root}" && cmake --build build --target clean)
fi

echo -e "\nBuilding the project...\n"
(cd "${repo_root}" && cmake --build build)
