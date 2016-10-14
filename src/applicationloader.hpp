#ifndef WEBMVCPP_APPLICATION_CLIENT_H
#define WEBMVCPP_APPLICATION_CLIENT_H

namespace webmvcpp
{
    class webapplication;

    extern "C" {

        typedef bool(*initModuleFn)(const char *webAppPath, const char *staticPath);
        typedef void *(*webmvcppInstanceFn)();
        typedef void(*deinitModuleFn)();
    }

    class webapplication_module
    {
    public:
        webapplication_module(const char *mdlPath, const char *appPath, const char *staticPath)
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

                initModule(appPath, staticPath);
                appInstance = (webapplication *)webmvcppInstance();
            }
        }
        virtual ~webapplication_module()
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

        webapplication *
        instance()
        {
            return appInstance;
        }

        unsigned long reference = 0;

    private:
        void *
        get_function(const char *fnName)
        {
#ifdef _WIN32
            void *fn = (void *)::GetProcAddress((HMODULE)moduleInstance, fnName);
            return fn;
#else // unix
            void *fn = dlsym(moduleInstance, fnName);
            return fn;
#endif
        }

        initModuleFn initModule;
        webmvcppInstanceFn webmvcppInstance;
        deinitModuleFn deinitModule;

        webapplication *appInstance;
        void *moduleInstance;
    };

    typedef boost::intrusive_ptr<webapplication_module> webapplication_module_ptr;
}

#endif // WEBMVCPP_APPLICATION_CLIENT_H