#!/bin/bash

# which version to fetch
MAJOR="3.7"
MINOR="2"
PLATFORM="Linux-x86_64"

# assemble url for desired version
URL="https://cmake.org/files/v${MAJOR}/cmake-${MAJOR}.${MINOR}-${PLATFORM}.tar.gz"

ARCHIVE="$(basename ${URL})"
DIR="$(basename ${ARCHIVE} .tar.gz)"

HERMIT_TOP="$(git rev-parse --show-toplevel)"
HERMIT_CMAKE="${HERMIT_TOP}/cmake"
CMAKE_DIR="${HERMIT_CMAKE}/${DIR}"
CMAKE_DIR_REL="$(realpath --relative-to=${HERMIT_TOP} ${CMAKE_DIR})"

# make sure we're sourced, not executed
if [ "$0" = "$BASH_SOURCE" ]
then
	echo "You have to source this script:"
	echo "\$ . $0"
	exit
fi

# quit if already in path
echo "$PATH" | grep "${CMAKE_DIR_REL}" &>/dev/null && return

if [ ! -d "${CMAKE_DIR}" ]
then
	echo "-- Downloading CMake"
	wget -q --show-progress "${URL}" -O "${ARCHIVE}" ||
		(echo "Cannot download CMake"; return)

	echo "-- Unpacking CMake"
	tar -C "${HERMIT_CMAKE}" -xf "${ARCHIVE}" ||
		(echo "Cannot unpack CMake archive"; return)

	# delete temporary archive again
	rm -f "${ARCHIVE}"

	# add cmake dir to gitignore
	GITIGNORE="${HERMIT_TOP}/.gitignore"
	if ! grep "${CMAKE_DIR_REL}" "${GITIGNORE}" &>/dev/null
	then
		echo "${CMAKE_DIR_REL}/*" >> "${GITIGNORE}"
	fi

	echo "-- Local CMake v${MAJOR}.${MINOR} installed to ${CMAKE_DIR_REL}"
	echo "-- Next time you source this script, no download will be neccessary"
fi

export PATH="${CMAKE_DIR}/bin:${PATH}"
