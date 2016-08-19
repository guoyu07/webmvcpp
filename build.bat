echo|set /p= #define WEBMVCPP_BUILD_NUMBER > ./src/buildinfo.h
git rev-list --count HEAD >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_COMPILER_C "cl" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_COMPILER_CPP "cl" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_LINKER "link" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
nmake -f ./makefile.msvc
