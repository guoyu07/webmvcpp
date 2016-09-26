#!/bin/bash

rm ./webmvcpp
buildNumber=$(git rev-list --count HEAD)
echo "#define WEBMVCPP_BUILD_NUMBER $buildNumber" > ./src/buildinfo.h
if test -x "$(type -p clang)"; then 
    echo "#define WEBMVCPP_COMPILER_C \"clang\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_COMPILER_CPP \"clang++\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.h
    clang++ ./build.cpp -o ./build
    ./build
    rm ./build
    make -f ./makefile.clang
else
    echo "#define WEBMVCPP_COMPILER_C \"gcc\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_COMPILER_CPP \"g++\"" >> ./src/buildinfo.h
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.h
    g++ ./build.cpp -o ./build
    ./build
    rm ./build
    make -f ./makefile.gcc
fi
rm webmvcpp_headers_hpp_amalgamation.c
