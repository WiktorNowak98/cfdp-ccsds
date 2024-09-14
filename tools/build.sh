#!/bin/bash

script_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
repo_root="${script_root%/tools}"
build_dir="${repo_root}/build"

clean_run="0"
configure_cmake="0"

while [[ $# -gt 0 ]]; do
	case $1 in
	--gcc)
		cxx_compiler="g++"
		shift
		;;
	--clang)
		cxx_compiler="clang++"
		shift
		;;
	--no-cache)
		clean_run="1"
		shift
		;;
	--configure)
		configure_cmake="1"
		shift
		;;
	*)
		echo "Unknown option $1"
		exit 1
		;;
	esac
done

echo "Build configuration:"
echo "root          = ${repo_root}"
echo "build dir     = ${build_dir}"
echo "C++ compiler  = ${cxx_compiler}"

if [ "${configure_cmake}" -eq "1" ]; then
	echo -e "\nRunning fresh cmake configuration...\n"
	(cd "${repo_root}" && rm -rf build) || true

	cmake -G Ninja -D CMAKE_CXX_COMPILER="${cxx_compiler}" -S "${repo_root}" -B build
fi

if [ ! -f "${build_dir}/CMakeCache.txt" ]; then
	echo -e "\nBuild directory or CMakeCache.txt does not exist."
	echo "Please run the script with --configure option first."

	exit 1
fi

if [ "${clean_run}" -eq "1" ]; then
	echo -e "\nClearing the cache for fresh build...\n"

	(cd "${repo_root}" && cmake --build build --target clean)
fi

echo -e "\nBuilding the project...\n"
(cd "${repo_root}" && cmake --build build)
