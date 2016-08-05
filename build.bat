echo|set /p= #define WEBMVCPP_BUILD_NUMBER > ./src/buildinfo.h
git rev-list --count HEAD >> ./src/buildinfo.h
nmake -f ./makefile.msvc