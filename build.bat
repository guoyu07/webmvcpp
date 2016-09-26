del .\webmvcpp.exe
echo|set /p= #define WEBMVCPP_BUILD_NUMBER > ./src/buildinfo.h
git rev-list --count HEAD >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_COMPILER_C "cl" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_COMPILER_CPP "cl" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
echo|set /p= #define WEBMVCPP_LINKER "link" >> ./src/buildinfo.h
echo. >> ./src/buildinfo.h
cl build.cpp /EHsc /link /subsystem:CONSOLE
build.exe
@del build.exe
@del build.obj
nmake -f ./makefile.msvc
@del webmvcpp_headers_hpp_amalgamation.c
