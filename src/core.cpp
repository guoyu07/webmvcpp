#include "webmvcpp.h" 
#include "applicationloader.h"

#if defined (_WIN32)
SERVICE_STATUS webmvcpp::core::serviceStatus = { 0 };
SERVICE_STATUS_HANDLE webmvcpp::core::statusHandle = NULL;
#endif

using namespace nlohmann;

namespace webmvcpp
{
#if defined (_WIN32)
	char *service_name = "webmvcpp_service";
#endif
    application* Application = NULL;

    core::core() :
    httpServer(new http_server_impl(this)),
    requestManager(this)
    {
#if defined (_WIN32)
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    }

    void
    core::application_unload(application *mvcApp)
    {
    }

	webapplication_ptr
    core::load_application(const char *path, const char *webAppPath, const char *staticPath)
    {
        webapplication_ptr cli = new webapplication(path, webAppPath, staticPath);

        return cli;
    }

#if defined (_WIN32)
	unsigned long WINAPI core::service_worker_thread(void *lpParam)
#else
	unsigned long core::service_worker_thread(void *lpParam)
#endif
	{
		core *webMvcCore = static_cast<core *>(lpParam);

		webMvcCore->start(false, 0, NULL);

		return 0;
	}

#if defined (_WIN32)
	void WINAPI core::service_ctrl_handler(DWORD ctrlCode)
	{
		switch (ctrlCode)
		{
		case SERVICE_CONTROL_STOP:

			if (serviceStatus.dwCurrentState != SERVICE_RUNNING)
				break;

			serviceStatus.dwControlsAccepted = 0;
			serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
			serviceStatus.dwWin32ExitCode = 0;
			serviceStatus.dwCheckPoint = 4;

			::SetServiceStatus(statusHandle, &serviceStatus);

			break;

		default:
			break;
		}
	}

	VOID WINAPI core::service_main(DWORD argc, LPTSTR *argv)
	{
		DWORD status = E_FAIL;

		// Register our service control handler with the SCM
		statusHandle = RegisterServiceCtrlHandlerA(service_name, service_ctrl_handler);

		if (statusHandle == NULL)
		{
			printf("ServiceMain: RegisterServiceCtrlHandler returned error");
		}

		// Tell the service controller we are starting
		memset(&serviceStatus, 0, sizeof(serviceStatus));
		serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		serviceStatus.dwControlsAccepted = 0;
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwServiceSpecificExitCode = 0;
		serviceStatus.dwCheckPoint = 0;

		if (SetServiceStatus(statusHandle, &serviceStatus) == FALSE)
		{

		}

		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCheckPoint = 0;

		::SetServiceStatus(statusHandle, &serviceStatus);

		HANDLE hThread = CreateThread(NULL, 0, service_worker_thread, NULL, 0, NULL);

		::WaitForSingleObject(hThread, INFINITE);

		// Tell the service controller we are stopped
		serviceStatus.dwControlsAccepted = 0;
		serviceStatus.dwCurrentState = SERVICE_STOPPED;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwCheckPoint = 3;

		if (SetServiceStatus(statusHandle, &serviceStatus) == FALSE)
		{
			printf("ServiceMain: SetServiceStatus returned error");
		}

		return;
	}
#endif

	bool core::start_daemon(int argc, char *args[])
	{
#if defined (_WIN32)
		SERVICE_TABLE_ENTRYA ServiceTable[] =
		{
			{ service_name, (LPSERVICE_MAIN_FUNCTIONA)core::service_main },
			{ NULL, NULL }
		};

		if (StartServiceCtrlDispatcherA(ServiceTable) == FALSE)
		{
			return false;
		}
#else
		pid_t process_id = fork();
		if (process_id < 0)
		{
			printf("fork failed!\n");
			exit(1);
		}

		if (process_id > 0)
		{
			printf("process_id of child process %d \n", process_id);
			// return success in exit status
			exit(0);
		}

		umask(0);

		pid_t sid = setsid();
		if (sid < 0)
		{
			exit(1);
		}

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		core::service_worker_thread(this);
#endif
		return true;
	}

    bool core::init(const std::string & webapp)
    {
        sessionManager.enable(24 * 60 * 60);

		std::string projectName = "undefined";

		std::ifstream configFile(webapp);
		if (!configFile.is_open())
		    return false;

		configFile.seekg(0, configFile.end);
		std::streamoff length = configFile.tellg();
		configFile.seekg(0, configFile.beg);

		std::string configString;
		configString.resize((unsigned int)length);
		configFile.read(&configString[0], length);

		const auto configRoot = json::parse(configString);
		const json::const_iterator projectNameIt = configRoot.find("projectName");
		if (projectNameIt != configRoot.end()) {
			const auto projectValue = projectNameIt.value();
			projectName = projectValue.get<std::string>();
		}
		const json::const_iterator bindPortValueIt = configRoot.find("bindPort");
		if (bindPortValueIt != configRoot.end()) {
			const auto bindPortValue = bindPortValueIt.value();
			bindPort = bindPortValue.get<unsigned short>();
		}
		json::const_iterator maxFormContentValueIt = configRoot.find("maxFormContentSize");
		if (maxFormContentValueIt != configRoot.end()) {
			const auto maxFormContentValue = maxFormContentValueIt.value();
			maxFormContent = maxFormContentValue.get<unsigned long>();
		}
		json::const_iterator hostsIt = configRoot.find("hosts");
		if (hostsIt != configRoot.end()) {
			const auto hosts = hostsIt.value();
			for (json::const_iterator hostsIt = hosts.begin(); hostsIt != hosts.end(); ++hostsIt) {
				const std::string & webAppName = hostsIt.key();
				const auto & webAppConfig = hostsIt.value();

				webappconfig config = new webappconfig_impl();

				const json::const_iterator aliasesIt = webAppConfig.find("alias");
				if (aliasesIt != webAppConfig.end()) {
					const auto aliasesValue = aliasesIt.value();
					for (json::const_iterator aliasIt = aliasesValue.begin(); aliasIt != aliasesValue.end(); ++aliasIt) {
						const auto aliasObj = *aliasIt;
						config->aliases.push_back(aliasObj.get<std::string>());
					}
				}
				json::const_iterator moduleIt = webAppConfig.find("module");
				if (moduleIt != webAppConfig.end()) {
					const auto moduleValue = moduleIt.value();
					config->modulePath = moduleValue.get<std::string>();
				}
				json::const_iterator sessionEnabledIt = webAppConfig.find("sessionsEnabled");
				if (sessionEnabledIt != webAppConfig.end()) {
					const auto sessionEnabledValue = sessionEnabledIt.value();
					config->sessionsEnabled = sessionEnabledValue.get<bool>();
				}
				json::const_iterator sessionTimeoutIt = webAppConfig.find("sessionTimeout");
				if (sessionTimeoutIt != webAppConfig.end()) {
					const auto sessionTimeoutValue = sessionTimeoutIt.value();
					config->sessionTimeout = sessionTimeoutValue.get<unsigned long>();
				}
				json::const_iterator staticPathIt = webAppConfig.find("staticPath");
				if (staticPathIt != webAppConfig.end()) {
					const auto staticPathValue = staticPathIt.value();
					config->staticPath = staticPathValue.get<std::string>();
				}
				json::const_iterator webappPathPathIt = webAppConfig.find("webappPath");
				if (webappPathPathIt != webAppConfig.end()) {
					const auto webappPathPathValue = webappPathPathIt.value();
					config->webappPath = webappPathPathValue.get<std::string>();
				}
				if (config->staticPath.length() == 0) {
					config->staticPath = get_parent_directory(config->modulePath) + "/static";
				}

				if (config->webappPath.length() == 0) {
					config->webappPath = get_parent_directory(config->modulePath) + "/webapp";
				}
				webApplicationConfigs.insert(std::pair<const std::string, webappconfig>(webAppName, config));
			}
		}

    	for (std::map<std::string, webappconfig>::iterator cIt = webApplicationConfigs.begin(); cIt != webApplicationConfigs.end(); ++cIt)
        {
            webapplication_ptr mdl = load_application(cIt->second->modulePath.c_str(), cIt->second->webappPath.c_str(), cIt->second->staticPath.c_str());
	        if (mdl != NULL)
	        {
	        	mdl->instance()->acceptCore(this);
		        webApps.insert(std::pair<std::string, webapplication_ptr>(cIt->first, mdl));
		        for (std::list<std::string>::iterator aliasesIt = cIt->second->aliases.begin(); aliasesIt != cIt->second->aliases.end(); ++aliasesIt) 
		        {
	                webApps.insert(std::pair<std::string, webapplication_ptr>(*aliasesIt, mdl));
		        }
	        }
	    }

        return true;
    }

    bool core::start(bool runAsDaemon, int argc, char *args[])
    {
		if (runAsDaemon) {
			return this->start_daemon(argc, args);
		}
        return httpServer->start_listening(bindPort);
    }

    bool core::process_request(http_connection connection, http_request & request, http_response & response)
    {
        std::map<std::string, webapplication_ptr>::iterator it = webApps.find(request.host);
        if (it == webApps.end())
        {
            http_error(403, "Forbidden", "Access to this host is forbidden by default").fill_response(response);     

            connection->send_response_header(response);
            connection->send_response_content(response.content);
            connection->end_response();

            return false;
        }

        requestManager.process_request(it->second->instance(), connection, request, response);

        return true;
    }
}
