#!/bin/bash
buildNumber=$(git rev-list --count HEAD)
echo "#define WEBMVCPP_BUILD_NUMBER $buildNumber" > ./src/buildinfo.h
if test -x "$(type -p clang)"; then 
    make -f ./makefile.clang
else
    make -f ./makefile.gcc
fi

#git rev-list --count HEAD >> ./src/buildinfo.h
