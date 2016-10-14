#!/bin/bash

rm ./webmvcpp
buildNumber=$(git rev-list --count HEAD)
echo "#define WEBMVCPP_MAJOR_VERSION 0" > ./src/buildinfo.hpp
echo "#define WEBMVCPP_MINOR_VERSION 5" >> ./src/buildinfo.hpp
echo "#define WEBMVCPP_BUILD_NUMBER $buildNumber" >> ./src/buildinfo.hpp

if test -x "$(type -p clang)"; then 
    echo "#define WEBMVCPP_COMPILER_C \"clang\"" >> ./src/buildinfo.hpp
    echo "#define WEBMVCPP_COMPILER_CPP \"clang++\"" >> ./src/buildinfo.hpp
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.hpp
    clang++ ./build.cpp -o ./build
    ./build
    rm ./build
    make -f ./makefile.clang
else
    echo "#define WEBMVCPP_COMPILER_C \"gcc\"" >> ./src/buildinfo.hpp
    echo "#define WEBMVCPP_COMPILER_CPP \"g++\"" >> ./src/buildinfo.hpp
    echo "#define WEBMVCPP_LINKER \"clang\"" >> ./src/buildinfo.h
    g++ ./build.cpp -o ./build
    ./build
    rm ./build
    make -f ./makefile.gcc
fi
rm webmvcpp_headers_hpp_amalgamation.c
