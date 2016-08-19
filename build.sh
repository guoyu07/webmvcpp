#!/bin/bash
buildNumber=$(git rev-list --count HEAD)
echo "#define WEBMVCPP_BUILD_NUMBER $buildNumber" > ./src/buildinfo.h
if test -x "$(type -p clang)"; then 
    echo "#define WEBMVCPP_COMPILER_C \"clang\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_COMPILER_CPP \"clang++\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.h
    make -f ./makefile.clang
else
    echo "#define WEBMVCPP_COMPILER_C \"gcc\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_COMPILER_CPP \"g++\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.h
    make -f ./makefile.gcc
fi
