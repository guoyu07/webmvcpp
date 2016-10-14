del .\webmvcpp.exe
echo|set /p= #define WEBMVCPP_MAJOR_VERSION 0 > ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
echo|set /p= #define WEBMVCPP_MINOR_VERSION 5 >> ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
echo|set /p= #define WEBMVCPP_BUILD_NUMBER >> ./src/buildinfo.hpp
git rev-list --count HEAD >> ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
echo|set /p= #define WEBMVCPP_COMPILER_C "cl" >> ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
echo|set /p= #define WEBMVCPP_COMPILER_CPP "cl" >> ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
echo|set /p= #define WEBMVCPP_LINKER "link" >> ./src/buildinfo.hpp
echo. >> ./src/buildinfo.hpp
cl build.cpp /EHsc /link /subsystem:CONSOLE
build.exe
@del build.exe
@del build.obj
nmake -f ./makefile.msvc
@del webmvcpp_headers_hpp_amalgamation.c
