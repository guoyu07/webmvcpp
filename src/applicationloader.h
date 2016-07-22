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
		webapplication(const char *mdlPath, const char *appPath, const char *staticPath);
        virtual ~webapplication();
        application *instance();

    private:
		void *get_function(const char *fnName);

        initModuleFn initModule;
        webmvcppInstanceFn webmvcppInstance;
        deinitModuleFn deinitModule;

        application *appInstance;
        void *moduleInstance;
    };

    typedef boost::intrusive_ptr<webapplication> webapplication_ptr;
}

#endif // WEBMVCPP_APPLICATION_CLIENT_H