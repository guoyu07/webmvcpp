#ifdef _WIN32
#include <windows.h>
BOOL __stdcall DllMain(HINSTANCE hinstance, DWORD reason, LPVOID reserved)
{
    return TRUE;
}
#endif
#include "f145hcom.h"

webmvcpp::application* Application = NULL;

extern "C"
#ifdef _WIN32
    __declspec(dllexport)
#endif
bool init_module(const char *webAppPath, const char *staticPath)
{
	std::string w = webAppPath;
	std::string s = staticPath;

    Application = new webmvcpp::F145hCom();
    if (!Application->init(webAppPath, staticPath))
        return false;    

    return true;
}

extern "C"
#ifdef _WIN32
__declspec(dllexport)
#endif
void *webmvcpp_instance()
{
    return Application;
}

extern "C"
#ifdef _WIN32
    __declspec(dllexport)
#endif
void deinit_module()
{
    delete Application;
}


#include "application.cpp"
#include "utils.cpp"
