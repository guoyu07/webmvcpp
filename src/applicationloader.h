#ifndef WEBMVCPP_APPLICATION_CLIENT_H
#define WEBMVCPP_APPLICATION_CLIENT_H

namespace webmvcpp
{
    class application;

    extern "C" {

        typedef bool(*initModuleFn)(const char *webAppPath, const char *staticPath);
        typedef void *(*webmvcppInstanceFn)();
        typedef void(*deinitModuleFn)();
    }

    class webapplication : public webmvcobject
    {
    public:
		webapplication(const char *mdlPath, const char *appPath, const char *staticPath) :
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

				initModule(appPath, staticPath);
				appInstance = (application *)webmvcppInstance();
			}
		}
		virtual ~webapplication()
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

		application *
		instance()
		{
			return appInstance;
		}


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

        application *appInstance;
        void *moduleInstance;
    };

    typedef boost::intrusive_ptr<webapplication> webapplication_ptr;
}

#endif // WEBMVCPP_APPLICATION_CLIENT_H