#ifndef WEBMVCPP_CORE_H
#define WEBMVCPP_CORE_H


namespace webmvcpp
{   
    class application;
	class session_manager;

    struct http_request;
    struct http_response;

	class webappconfig
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

    class core : public core_prototype
    {
    public:
		core() :
		requestManager(this),
		sessionManager(),
		httpServer(this)
		{
			startTimestamp = std::time(NULL);
			srand((unsigned int)std::time(NULL));
#if defined (_WIN32)
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
		}

		const time_t get_start_timestamp() { return startTimestamp; }

		static const char *get_service_name() { return "webmvcpp_service"; }

#if defined (_WIN32)

		static SERVICE_STATUS & get_service_status() {
			static SERVICE_STATUS serviceStatus = { 0 };

			return serviceStatus;
		}
		static SERVICE_STATUS_HANDLE & get_service_status_handle() {
			static SERVICE_STATUS_HANDLE serviceStatus = NULL;

			return serviceStatus;
		}
#endif


        bool init(const std::string & webapp)
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

					webappconfig config;

					const json::const_iterator aliasesIt = webAppConfig.find("alias");
					if (aliasesIt != webAppConfig.end()) {
						const auto aliasesValue = aliasesIt.value();
						for (json::const_iterator aliasIt = aliasesValue.begin(); aliasIt != aliasesValue.end(); ++aliasIt) {
							const auto aliasObj = *aliasIt;
							config.aliases.push_back(aliasObj.get<std::string>());
						}
					}
					json::const_iterator moduleIt = webAppConfig.find("module");
					if (moduleIt != webAppConfig.end()) {
						const auto moduleValue = moduleIt.value();
						config.modulePath = moduleValue.get<std::string>();
					}
					json::const_iterator sessionEnabledIt = webAppConfig.find("sessionsEnabled");
					if (sessionEnabledIt != webAppConfig.end()) {
						const auto sessionEnabledValue = sessionEnabledIt.value();
						config.sessionsEnabled = sessionEnabledValue.get<bool>();
					}
					json::const_iterator sessionTimeoutIt = webAppConfig.find("sessionTimeout");
					if (sessionTimeoutIt != webAppConfig.end()) {
						const auto sessionTimeoutValue = sessionTimeoutIt.value();
						config.sessionTimeout = sessionTimeoutValue.get<unsigned long>();
					}
					json::const_iterator staticPathIt = webAppConfig.find("staticPath");
					if (staticPathIt != webAppConfig.end()) {
						const auto staticPathValue = staticPathIt.value();
						config.staticPath = staticPathValue.get<std::string>();
					}
					json::const_iterator webappPathPathIt = webAppConfig.find("webappPath");
					if (webappPathPathIt != webAppConfig.end()) {
						const auto webappPathPathValue = webappPathPathIt.value();
						config.webappPath = webappPathPathValue.get<std::string>();
					}
					if (config.staticPath.length() == 0) {
						config.staticPath = utils::get_parent_directory(config.modulePath) + "/static";
					}

					if (config.webappPath.length() == 0) {
						config.webappPath = utils::get_parent_directory(config.modulePath) + "/webapp";
					}
					webApplicationConfigs.insert(std::pair<const std::string, webappconfig>(webAppName, config));
				}
			}

			for (std::map<std::string, webappconfig>::iterator cIt = webApplicationConfigs.begin(); cIt != webApplicationConfigs.end(); ++cIt)
			{
				if (cIt->second.modulePath.length() == 0)
					continue;

				webapplication_ptr mdl = load_application(cIt->second.modulePath.c_str(), cIt->second.webappPath.c_str(), cIt->second.staticPath.c_str());
				if (mdl->instance() != NULL)
				{
					mdl->instance()->acceptCore(this);
					webApps.insert(std::pair<std::string, webapplication_ptr>(cIt->first, mdl));
					for (std::list<std::string>::iterator aliasesIt = cIt->second.aliases.begin(); aliasesIt != cIt->second.aliases.end(); ++aliasesIt)
					{
						webApps.insert(std::pair<std::string, webapplication_ptr>(*aliasesIt, mdl));
					}
				}
			}

			return true;
		}


		bool start(bool runAsDaemon, int argc, char *args[])
		{
			if (runAsDaemon) {
				return this->start_daemon(argc, args);
			}
			return httpServer.start_listening(bindPort);
		}

        void stop();

        void application_unload(application *mvcApp) {

		}
		webapplication_ptr load_application(const char *path, const char *webAppPath, const char *staticPath)
		{
			webapplication_ptr cli = new webapplication(path, webAppPath, staticPath);

			return cli;
		}

		virtual mime_file_types *get_mime_types() { return &mimeTypes; }
		virtual session_manager *get_session_manager() { return &sessionManager; }

		virtual bool process_request(http_connection *connection, http_request & request, http_response & response)
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
		void build_applications()
		{
			for (std::map<std::string, webappconfig>::iterator cIt = webApplicationConfigs.begin(); cIt != webApplicationConfigs.end(); ++cIt) 
			{
				if (cIt->second.modulePath.length() == 0 && cIt->second.webappPath.length() != 0)
				{
					std::string buildedAppPath;
					if (build_application(cIt->first, cIt->second.webappPath, buildedAppPath)) {
						cIt->second.modulePath = buildedAppPath;
					}
				}
			}
		}

	private:
		bool build_application(const std::string & appName, std::string & webAppPath, std::string & resultPath)
		{
			std::list<std::string> sourcefiles;

			DIR *webappDir = opendir(webAppPath.c_str());
			if (!webappDir)
				return false;

			while (dirent *entry = readdir(webappDir))
			{
				if (entry->d_type == DT_REG) {

					std::vector<std::string> splittedFile = utils::split_string(entry->d_name, '.');
					if (splittedFile.size() < 2)
						continue;
					std::string fileExtension = splittedFile[splittedFile.size() - 1];
					if (fileExtension == "cpp" || fileExtension == "c")
						sourcefiles.push_back(webAppPath + "/" + entry->d_name);
				}
			}
			closedir(webappDir);

			std::string controllersPath = webAppPath + "/controllers";

			DIR *controllersDir = opendir(controllersPath.c_str());
			if (!controllersDir)
				return false;

			while (dirent *entry = readdir(controllersDir))
			{
				if (entry->d_type == DT_REG) {
					
					std::vector<std::string> splittedFile = utils::split_string(entry->d_name, '.');
					if (splittedFile.size() < 2)
						continue;
					std::string fileExtension = splittedFile[splittedFile.size() - 1];
					if (fileExtension == "cpp" || fileExtension == "c")
						sourcefiles.push_back(controllersPath + "/" + entry->d_name);
				}
			}
			closedir(controllersDir);

			builder webAppbuilder(appName, webAppPath);
			std::list<std::string> objFiles;

			for (std::list<std::string>::iterator sourceIt = sourcefiles.begin(); sourceIt != sourcefiles.end(); ++sourceIt) {
				
				std::string result;
				std::string tmpObjName = tmpnam(NULL);
				if (!webAppbuilder.compile(*sourceIt, tmpObjName, result)) {
					return false;
				}

				objFiles.push_back(tmpObjName);
			}

			std::string linkAppResult;
			std::string tmpWebAppFile = tmpnam(NULL);
#if defined (_WIN32)
			std::string defFile;
			std::string tmpDefFile = tmpnam(NULL);
			webAppbuilder.generateApplicationDef(tmpDefFile);
			bool buildResult = webAppbuilder.linkApplication(objFiles, tmpDefFile, tmpWebAppFile, linkAppResult);
			::DeleteFileA(tmpDefFile.c_str());
#else
			bool buildResult = webAppbuilder.linkApplication(objFiles, tmpWebAppFile, linkAppResult);
#endif
			if (buildResult)
				resultPath = tmpWebAppFile;

			return buildResult;
		}

		bool start_daemon(int argc, char *args[])
		{
#if defined (_WIN32)
			static SERVICE_STATUS serviceStatus = { 0 };
			static SERVICE_STATUS_HANDLE statusHandle = NULL;

			SERVICE_TABLE_ENTRYA ServiceTable[] =
			{
				{ (LPSTR)get_service_name(), (LPSERVICE_MAIN_FUNCTIONA)core::service_main },
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
#if defined (_WIN32)
		static void WINAPI service_ctrl_handler(DWORD ctrlCode)
		{
			switch (ctrlCode)
			{
			case SERVICE_CONTROL_STOP:
			{
				SERVICE_STATUS & serviceStatus = get_service_status();

				if (serviceStatus.dwCurrentState != SERVICE_RUNNING)
					break;

				serviceStatus.dwControlsAccepted = 0;
				serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
				serviceStatus.dwWin32ExitCode = 0;
				serviceStatus.dwCheckPoint = 4;

				::SetServiceStatus(get_service_status_handle(), &serviceStatus);

				break;
			}
			default:
				break;
			}
		}

		static VOID WINAPI service_main(DWORD argc, LPTSTR *argv)
		{
			DWORD status = E_FAIL;

			SERVICE_STATUS_HANDLE & statusHandle = get_service_status_handle();
			// Register our service control handler with the SCM
			statusHandle = RegisterServiceCtrlHandlerA(get_service_name(), service_ctrl_handler);

			if (statusHandle == NULL)
			{
				printf("ServiceMain: RegisterServiceCtrlHandler returned error");
			}

			SERVICE_STATUS & serviceStatus = get_service_status();

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
#if defined (_WIN32)
		static unsigned long WINAPI service_worker_thread(void *lpParam)
#else
		static unsigned long service_worker_thread(void *lpParam)
#endif
		{
			core *webMvcCore = static_cast<core *>(lpParam);

			webMvcCore->start(false, 0, NULL);

			return 0;
		}
	    
        std::map<std::string, webapplication_ptr> webApps;
		std::map<std::string, webappconfig> webApplicationConfigs;

        request_manager requestManager;
		session_manager sessionManager;
        mime_file_types mimeTypes;
		http_server httpServer;

		unsigned short bindPort = 8080;
		unsigned long maxFormContent = 1 * 1024 * 1024;

		time_t startTimestamp;
        std::map<std::string, std::string> routeMap;
    };
}

#endif // WEBMVCPP_CORE_H
