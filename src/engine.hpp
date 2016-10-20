#ifndef WEBMVCPP_CORE_H
#define WEBMVCPP_CORE_H


namespace webmvcpp
{
    class webapplication;
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
        std::string binaryOutputDirectory;
        std::list<std::string> includeDirectories;
        std::list<std::string> linkLibraries;
        std::list<std::string> definitions;

        nlohmann::json rawObject;
    };

    class engine : public core_prototype
    {
    public:
        engine() :
        requestManager(this),
        sessionManager(),
        httpServer(this)
        {
            startTimestamp = std::time(NULL);
            srand((unsigned int)std::time(NULL));
#if defined (_WIN32)
            network::initialize();
#endif
        }

        const time_t get_start_timestamp() { return startTimestamp; }

        static const char *get_service_name() { return "webmvcpp_service"; }

        virtual void log(const std::string &logMessage)
        {
            std::cout << logMessage << std::endl;
        }

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
        void display_version()
        {
            std::cout << "WebMVC++ Open Source Web Application Engine" << std::endl;
            std::cout << "Version: " << WEBMVCPP_MAJOR_VERSION << "." << WEBMVCPP_MINOR_VERSION << "." << WEBMVCPP_BUILD_NUMBER << " " << __DATE__ << " at " << __TIME__ << std::endl;
            std::cout << "Compiler: " << WEBMVCPP_COMPILER_CPP << std::endl;
        }

        void install()
        {
            std::string srcPath = webmvcpp::systemutils::getApplicationPath();
            std::string targetPath;
#ifdef _WIN32
            char sysDirBuffer[MAX_PATH];
            sysDirBuffer[GetSystemDirectoryA(sysDirBuffer, sizeof(sysDirBuffer))] = 0;
            targetPath = sysDirBuffer;
            targetPath += "\\webmvcpp.exe";
#else
            targetPath = "/usr/local/bin/webmvcpp";
#endif
            std::cout << "Installing " << srcPath << " to " << targetPath << std::endl;
            std::ifstream  src(srcPath, std::ios::binary);
            std::ofstream  dst(targetPath,  std::ios::binary);
#ifndef _WIN32
            chmod("/usr/local/bin/webmvcpp", S_IRWXU|S_IRWXG|S_IROTH|S_IWOTH);
#endif
            dst << src.rdbuf() <<std::flush;
            std::cout << "Success" << std::endl;
            dst.close();
            src.close();
        }

        void uninstall()
        {
#ifdef _WIN32
            std::string targetPath;
            char sysDirBuffer[MAX_PATH];
            sysDirBuffer[GetSystemDirectoryA(sysDirBuffer, sizeof(sysDirBuffer))] = 0;
            targetPath = sysDirBuffer;
            targetPath += "\\webmvcpp.exe";
            ::DeleteFileA(targetPath.c_str());
#else
            ::remove("/usr/local/bin/webmvcpp");
#endif
            std::cout << "Success" << std::endl;
        }

        void test_configuration(const std::string & configPath)
        {
            std::ifstream configFile(configPath);
            if (!configFile.is_open())
            {
                std::cout << "could not open " << configPath << std::endl;
                return;
            }

            configFile.seekg(0, configFile.end);
            std::streamoff length = configFile.tellg();
            configFile.seekg(0, configFile.beg);

            std::string configString;
            configString.resize((unsigned int)length);
            configFile.read(&configString[0], length);

            try {
                const auto configRoot = json::parse(configString.c_str());
                std::cout << "success" << std::endl;
            }
            catch(...){
                std::cout << "failed" << std::endl;
            }
        }

        void create_application(const std::string & appName)
        {
            builder webAppbuilder(appName);
            webAppbuilder.create_application();
        }

        bool load_web_application(const std::string & name, const std::list<std::string> & aliases,const std::string & modulePath, const std::string & webAppPath, const std::string & staticPath)
        {
            std::cout << "load module: " << modulePath;
            webapplication_module_ptr mdl = new webapplication_module(modulePath.c_str(), webAppPath.c_str(), staticPath.c_str());
            if (mdl->instance() != NULL)
            {
                std::cout << " success" << std::endl;
                mdl->instance()->acceptCore(this);
                webApps.insert(std::pair<std::string, webapplication_module_ptr>(name, mdl));
                for (std::list<std::string>::const_iterator aliasesIt = aliases.begin(); aliasesIt != aliases.end(); ++aliasesIt)
                {
                    webApps.insert(std::pair<std::string, webapplication_module_ptr>(*aliasesIt, mdl));
                }

                return true;
            }
            else {
                std::cout << " fail" << std::endl;
            }

            return false;
        }

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

            const auto configRoot = json::parse(configString.c_str());

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
                maxFormContentSize = maxFormContentValue.get<unsigned long>();
            }
            json::const_iterator maxConnectionsValueIt = configRoot.find("maxConnections");
            if (maxConnectionsValueIt != configRoot.end()) {
                const auto maxConnectionsValue = maxConnectionsValueIt.value();
                maxConnections = maxConnectionsValue.get<unsigned long>();
            }
            json::const_iterator connectionsPerIPValueIt = configRoot.find("connectionsPerIP");
            if (connectionsPerIPValueIt != configRoot.end()) {
                const auto connectionsPerIPValue = connectionsPerIPValueIt.value();
                connectionsPerIp = connectionsPerIPValue.get<unsigned long>();
            }
            json::const_iterator reqestTimeoutValueIt = configRoot.find("reqestTimeout");
            if (reqestTimeoutValueIt != configRoot.end()) {
                const auto reqestTimeoutValue = reqestTimeoutValueIt.value();
                requestTimeout = reqestTimeoutValue.get<unsigned long>();
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
                        if (moduleIt->is_string()) {
                            const auto moduleValue = moduleIt.value();
                            config.modulePath = moduleValue.get<std::string>();
                        }
                        else if (moduleIt->is_object()) {
                            const auto & buildMdlParams = moduleIt.value();
                            const json::const_iterator webappPathIt = buildMdlParams.find("webappPath");
                            if (webappPathIt != buildMdlParams.end()) {
                                const auto webappPathValue = webappPathIt.value();
                                config.webappPath = webappPathValue.get<std::string>();
                            }
                            const json::const_iterator binaryOutputDirectoryIt = buildMdlParams.find("binaryOutputDirectory");
                            if (binaryOutputDirectoryIt != buildMdlParams.end()) {
                                const auto binaryOutputDirectoryValue = binaryOutputDirectoryIt.value();
                                config.binaryOutputDirectory = binaryOutputDirectoryValue.get<std::string>();
                            }
                            const json::const_iterator includeDirectoriesIt = buildMdlParams.find("includeDirectories");
                            if (includeDirectoriesIt != buildMdlParams.end()) {
                                const auto includeDirectoriesValue = includeDirectoriesIt.value();
                                for (json::const_iterator includeDirectoryIt = includeDirectoriesValue.begin(); includeDirectoryIt != includeDirectoriesValue.end(); ++includeDirectoryIt) {
                                    const auto includeDirectoryObj = *includeDirectoryIt;
                                    config.includeDirectories.push_back(includeDirectoryObj.get<std::string>());
                                }
                            }
                            const json::const_iterator linkLibrariesIt = buildMdlParams.find("linkLibraries");
                            if (linkLibrariesIt != buildMdlParams.end()) {
                                const auto linkLibrariesValue = linkLibrariesIt.value();
                                for (json::const_iterator linkLibraryIt = linkLibrariesValue.begin(); linkLibraryIt != linkLibrariesValue.end(); ++linkLibraryIt) {
                                    const auto linkLibraryObj = *linkLibraryIt;
                                    config.linkLibraries.push_back(linkLibraryObj.get<std::string>());
                                }
                            }
                            const json::const_iterator definitionsIt = buildMdlParams.find("definitions");
                            if (definitionsIt != buildMdlParams.end()) {
                                const auto definitionsValue = definitionsIt.value();
                                for (json::const_iterator defineIt = definitionsValue.begin(); defineIt != definitionsValue.end(); ++defineIt) {
                                    const auto defineObj = *defineIt;
                                    config.definitions.push_back(defineObj.get<std::string>());
                                }
                            }
                        }
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

                load_web_application(cIt->first, cIt->second.aliases, cIt->second.modulePath, cIt->second.webappPath, cIt->second.staticPath);
            }

            return true;
        }


        bool start(bool runAsDaemon, int argc, char *args[])
        {
            if (runAsDaemon) {
                return this->start_daemon(argc, args);
            }
            return httpServer.start_listening(bindPort, maxConnections, connectionsPerIp, requestTimeout);
        }

        void stop()
       {

        }

        void application_unload(webapplication *mvcApp) {

        }

        virtual mime_file_types_prototype *get_mime_types() { return &mimeTypes; }
        virtual session_manager *get_session_manager() { return &sessionManager; }

        virtual bool process_request(http_incoming_connection *connection, http_request & request, http_response & response)
        {
            std::map<std::string, webapplication_module_ptr>::iterator it = webApps.find(request.host);
            if (it == webApps.end())
            {
                error_page::send(response, 403, "Forbidden", "Access to this host is forbidden by default");
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
                    std::cout << "build application: " << cIt->first << std::endl;
                    if (build_application(cIt->first, cIt->second.webappPath, cIt->second.definitions, cIt->second.includeDirectories, cIt->second.linkLibraries, buildedAppPath)) {
                        cIt->second.modulePath = buildedAppPath;
                        std::cout << "application module: " << buildedAppPath << std::endl;
                    }
                }
            }
        }

        void load_applications()
        {
            for (std::map<std::string, webappconfig>::iterator cIt = webApplicationConfigs.begin(); cIt != webApplicationConfigs.end(); ++cIt)
            {
                if (cIt->second.modulePath.length() != 0 && cIt->second.webappPath.length() != 0)
                {
                    std::string buildedAppPath;
                    std::cout << "load application: " << cIt->first << std::endl;

                    load_web_application(cIt->first, cIt->second.aliases, cIt->second.modulePath, cIt->second.webappPath, cIt->second.staticPath);
                }
            }
        }

    private:
        bool build_application(const std::string & appName, std::string & webAppPath, const std::list<std::string> & definitions, const std::list<std::string> & includeDirectories, const std::list<std::string> & linkLibraries, std::string & resultPath)
        {
            bool buildResult = true;
            std::list<std::string> sourcefiles;

            builder webAppbuilder(appName, webAppPath);

            DIR *webappDir = opendir(webAppPath.c_str());
            if (!webappDir)
                return false;

            webAppbuilder.generateViews(webAppPath + "/webmvcpp_views.cpp");
            webAppbuilder.generateModels (webAppPath + "/webmvcpp_models.cpp");
            webAppbuilder.generateControllers(webAppPath + "/webmvcpp_controllers.cpp");

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

            std::list<std::string> objFiles;

#ifndef _WIN32
            int fileNumber = 0;
            char templateDirectoryFmt[] = "/tmp/webmvcppXXXXXX";
            std::string tmpDirectoryPath = ::mkdtemp(templateDirectoryFmt);
            ::mkdir(tmpDirectoryPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
            for (std::list<std::string>::iterator sourceIt = sourcefiles.begin(); sourceIt != sourcefiles.end(); ++sourceIt) {

                std::string result;
#ifdef _WIN32
                char tmpFileNameBuffer[MAX_PATH];
                tmpnam_s(tmpFileNameBuffer, sizeof(tmpFileNameBuffer));
                std::string tmpObjName = tmpFileNameBuffer;
#else
                std::ostringstream tmpObjNameStrm;
                tmpObjNameStrm << tmpDirectoryPath << "/" << ++fileNumber;
                std::string tmpObjName = tmpObjNameStrm.str();
#endif
                if (!webAppbuilder.compile(*sourceIt, definitions, includeDirectories, tmpObjName, result)) {
                    buildResult = false;
                    break;
                }

                objFiles.push_back(tmpObjName);
            }

            if (buildResult) {
                std::string linkAppResult;
                std::string webAppFile = webAppPath + "/" + appName;
#if defined (_WIN32)
                webAppFile += ".dll";
                std::string defFile = webAppPath + "/" + appName + ".def";
                buildResult = webAppbuilder.linkApplication(objFiles, linkLibraries, defFile, webAppFile, linkAppResult);
#else
                webAppFile += ".so";
                buildResult = webAppbuilder.linkApplication(objFiles, linkLibraries, webAppFile, linkAppResult);
#endif
                if (buildResult)
                    resultPath = webAppFile;
            }

            for (std::list<std::string>::const_iterator objIt = objFiles.begin(); objIt != objFiles.end(); ++objIt)
            {
#if defined (_WIN32)
                ::DeleteFileA(objIt->c_str());
#else
                remove(objIt->c_str());
#endif
            }
#ifndef _WIN32
            ::rmdir(tmpDirectoryPath.c_str());
#endif


            return buildResult;
        }

        bool start_daemon(int argc, char *args[])
        {
#if defined (_WIN32)
            static SERVICE_STATUS serviceStatus = { 0 };
            static SERVICE_STATUS_HANDLE statusHandle = NULL;

            SERVICE_TABLE_ENTRYA ServiceTable[] =
            {
                { (LPSTR)get_service_name(), (LPSERVICE_MAIN_FUNCTIONA)engine::service_main },
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
                this->log("fork failed!\n");
                exit(1);
            }

            if (process_id > 0)
            {
                this->log("bad process_id value");
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

            engine::service_worker_thread(this);
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
            engine *webMvcCore = static_cast<engine *>(lpParam);

            webMvcCore->start(false, 0, NULL);

            return 0;
        }
        
        std::map<std::string, webapplication_module_ptr> webApps;
        std::map<std::string, webappconfig> webApplicationConfigs;

        request_manager requestManager;
        session_manager sessionManager;
        mime_file_types mimeTypes;
        http_server httpServer;

        unsigned short bindPort = 8081;
        unsigned long maxFormContentSize = 1 * 1024 * 1024;
        unsigned long maxConnections = 1000;
        unsigned long connectionsPerIp = 100;
        unsigned long requestTimeout = 30;

        time_t startTimestamp;
        std::map<std::string, std::string> routeMap;
    };
}

#endif // WEBMVCPP_CORE_H
