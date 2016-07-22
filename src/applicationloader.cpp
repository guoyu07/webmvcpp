#include "webmvcpp.h"

namespace webmvcpp
{
	webapplication::webapplication(const char *mdlPath, const char *webAppPath, const char *staticPath) :
	webmvcobject()
    {
#ifdef _WIN32
        moduleInstance = ::LoadLibraryA(mdlPath);
#else // unix
        moduleInstance = dlopen(mdlPath, RTLD_LAZY);
#endif
        if (moduleInstance)
        {
            initModule = (initModuleFn)get_function("init_module");
            webmvcppInstance = (webmvcppInstanceFn)get_function("webmvcpp_instance");
            deinitModule = (deinitModuleFn)get_function("deinit_module");
			
            initModule(webAppPath, staticPath);
            appInstance = (application *)webmvcppInstance();
        }
    }

    void *
		webapplication::get_function(const char *fnName)
    {
#ifdef _WIN32
        void *fn = (void *)::GetProcAddress((HMODULE)moduleInstance, fnName);
        return fn;
#else // unix
        void *fn = dlsym(moduleInstance, fnName);
        return fn;
#endif
    }

    application *
	webapplication::instance()
    { 
        return appInstance; 
    }

	webapplication::~webapplication()
    {
        deinitModule();

        if (moduleInstance)
        {
#ifdef _WIN32
            ::FreeLibrary((HMODULE)moduleInstance);
#else // unix
            dlclose(moduleInstance);
#endif
        }
    }
}