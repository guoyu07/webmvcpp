#ifndef WEBMVCPP_CORE_H
#define WEBMVCPP_CORE_H

#include "connection.h"
#include "server.h"
#include "requestmanager.h"
#include "sessionmanager.h"
#include "applicationloader.h"



namespace webmvcpp
{   
    class application;

    struct http_request;
    struct http_response;

	class webappconfig_impl : public webmvcobject 
	{
	public:
		std::list<std::string> aliases;
		std::string modulePath;
		bool sessionsEnabled = false;
		unsigned long sessionTimeout = 24;
		std::string staticPath;
		std::string webappPath;

		nlohmann::json rawObject;
	};
	typedef boost::intrusive_ptr<webappconfig_impl> webappconfig;

    class core
    {
    public:
        core();

#if defined (_WIN32)
		static SERVICE_STATUS serviceStatus;
		static SERVICE_STATUS_HANDLE statusHandle;
#endif

        bool init(const std::string & webapp);

        bool start(bool runAsDaemon, int argc, char *args[]);
        void stop();

        void application_unload(application *mvcApp);
        webapplication_ptr load_application(const char *path, const char *webAppPath, const char *staticPath);

        void build_module();

		mime_file_types & mimeTypes() { return mimeFileTypes; }
		requests_session_manager & session_manager() { return sessionManager; }

        bool process_request(http_connection connection, http_request & request, http_response & response);

	private:
		bool start_daemon(int argc, char *args[]);
#if defined (_WIN32)
		static VOID WINAPI service_main(DWORD argc, LPTSTR *argv);
		static void WINAPI service_ctrl_handler(DWORD ctrlCode);
#endif
#if defined (_WIN32)
		static unsigned long WINAPI service_worker_thread(void *lpParam);
#else
		static unsigned long service_worker_thread(void *lpParam);
#endif
	    http_server httpServer;
		unsigned short bindPort = 8080;
		unsigned long maxFormContent = 1 * 1024 * 1024;

        std::map<std::string, webapplication_ptr> webApps;
		std::map<std::string, webappconfig> webApplicationConfigs;

        request_manager requestManager;

        requests_session_manager sessionManager;

        mime_file_types mimeFileTypes;

        std::map<std::string, std::string> routeMap;
    };
}

#endif // WEBMVCPP_CORE_H
