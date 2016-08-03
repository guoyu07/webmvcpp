#ifndef WEBMVCPP_APPLICATION_MODULE_H
#define WEBMVCPP_APPLICATION_MODULE_H


namespace webmvcpp
{
    struct mvc_handlers{
        
        webmvcpp_start_application_fn startApplication;
        webmvcpp_stop_application_fn stopApplication;

        webmvcpp_create_session_fn createSession;
        webmvcpp_remove_session_fn removeSession;

        webmvcpp_check_authorized_fn checkAuthorized;

        webmvcpp_view_handler masterPageHandler;

        std::map<std::string, webmvcpp_view_handler> views;
        std::map<std::string, webmvcpp_request_handler> requests;

        static mvc_handlers *g;
        static mvc_handlers *global() {
            if (g == NULL) {
                g = new mvc_handlers();
            }
            return g;
        };

		~mvc_handlers() {}
    };

	#define WEBMVC_VIEWDATA "<webmvcpp:content:"
	#define WEBMVC_VIEWDATA_CLOSED "</webmvcpp:content:"
	#define WEBMVC_CLOSEBLOCK_END " />"
	#define WEBMVC_BLOCK_END ">"

    class application
    {

    public:
		application() {}
		virtual ~application() {}

		virtual void start() {}
		virtual void stop() {}

        void acceptCore(core *c){ mvcCore = c; }

		virtual bool init(const std::string & w, const std::string & s)
		{
			webappPath = w;
			staticPath = s;

			this->init_models();
			this->init_controllers();
			this->init_pages();

			return true;
		}

        mvc_handlers *handlers = mvc_handlers::global();

        std::map<std::string, std::string> pages;
        std::set<std::string> controllers;
        std::map<std::string, std::map<std::string, request_model>> reqModels;
        
        std::string webappPath;
        std::string staticPath;

        core *mvcCore;

        std::map<std::string, std::string> routeMap;

		void add_route(const std::string & path, const std::string & newPath)
		{
			routeMap.insert(std::pair<std::string, std::string>(path, newPath));
		}

		bool redirect_to(http_response & response, const std::string & url)
		{
			response.status = "302 found";
			response.header.insert(std::pair<std::string, std::string>("Location", url));

			return true;
		}

        virtual bool get_masterpage(std::string & pageFile)
		{
			std::ifstream masterPageFile(webappPath + "/views/master.html");
			if (!masterPageFile.is_open())
				return false;

			masterPageFile.seekg(0, std::ios::end);
			std::streampos fileSize = masterPageFile.tellg();
			masterPageFile.seekg(0, std::ios::beg);

			pageFile.resize((unsigned int)fileSize);
			masterPageFile.read(&pageFile[0], fileSize);

			return true;
		}

		bool init_models()
		{
			std::string path = webappPath + "/models";
			DIR *currentDir = opendir(path.c_str());
			if (!currentDir)
				return false;

			std::list<std::string> entries;

			while (dirent *entry = readdir(currentDir))
			{
				if (entry->d_type == DT_REG)
					entries.push_back(entry->d_name);
			}
			closedir(currentDir);

			for (std::list<std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it)
			{
				std::map<std::string, request_model> reqModelMap;

				const std::string & modelControllerName = *it;

				std::ifstream controllerModel(path + "/" + modelControllerName, std::ios::in | std::ios::binary);
				if (!controllerModel.is_open())
					continue;

				controllerModel.seekg(0, std::ios::end);
				std::streampos fileSize = controllerModel.tellg();
				controllerModel.seekg(0, std::ios::beg);

				std::string controllerModelJsonStr;
				controllerModelJsonStr.resize((unsigned int)fileSize);
				controllerModel.read(&controllerModelJsonStr[0], fileSize);

				const auto jsonObhect = json::parse(controllerModelJsonStr);

				for (json::const_iterator it = jsonObhect.begin(); it != jsonObhect.end(); ++it)
				{
					const std::string & methodName = it.key();
					const auto & methodObj = it.value();

					for (json::const_iterator methodsIt = methodObj.begin(); methodsIt != methodObj.end(); ++methodsIt)
					{
						const std::string & reqTypeName = methodsIt.key();

						if (reqTypeName == "GET")
						{
							request_model reqModel;

							const auto & reqModelObj = methodsIt.value();

							for (json::const_iterator mdlIt = reqModelObj.begin(); mdlIt != reqModelObj.end(); ++mdlIt)
							{
								const std::string & mdlProperty = mdlIt.key();

								if (mdlProperty == MVCPP_MODEL_KEY_FLAGS)
								{
									const auto & flags = mdlIt.value();
									for (json::const_iterator flagIt = flags.begin(); flagIt != flags.end(); ++flagIt)
									{
										const auto propertyValue = *flagIt;
										if (propertyValue.is_string()) {
											reqModel.flags.insert(propertyValue.get<std::string>());
										}
									}
								}
								else if (mdlProperty == MVCPP_MODEL_KEY_QUERY_STRING)
								{
									const auto & keys = mdlIt.value();
									for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
									{
										const auto propertyValue = *keysIt;
										if (propertyValue.is_string()) {
											reqModel.queryString.push_back(propertyValue.get<std::string>());
										}
									}
								}
							}

							reqModelMap.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
						}
						else if (reqTypeName == "POST")
						{
							request_model reqModel;
							const auto & reqModelObj = methodsIt.value();

							for (json::const_iterator mdlIt = reqModelObj.begin(); mdlIt != reqModelObj.end(); ++mdlIt)
							{
								const std::string & mdlProperty = mdlIt.key();

								if (mdlProperty == MVCPP_MODEL_KEY_FLAGS)
								{
									const auto & flags = *mdlIt;
									for (json::const_iterator flagIt = flags.begin(); flagIt != flags.end(); ++flagIt)
									{
										const auto propertyValue = *flagIt;
										if (propertyValue.is_string()) {
											reqModel.flags.insert(propertyValue.get<std::string>());
										}
									}
								}
								else if (mdlProperty == MVCPP_MODEL_KEY_CONTENT_TYPE)
								{
									const auto propertyValue = mdlIt.value();
									if (propertyValue.is_string()) {
										reqModel.contentType = propertyValue.get<std::string>();
									}
								}
								else if (mdlProperty == MVCPP_MODEL_KEY_QUERY_STRING)
								{
									const auto keys = mdlIt.value();
									for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
									{
										const auto propertyValue = keysIt.value();
										if (propertyValue.is_string()) {
											reqModel.queryString.push_back(propertyValue.get<std::string>());
										}
									}
								}
								else if (mdlProperty == MVCPP_MODEL_KEY_BODY_ENCODED_PARAMS)
								{
									const auto keys = mdlIt.value();
									for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
									{
										const auto propertyValue = keysIt.value();
										if (propertyValue.is_string()) {
											reqModel.bodyUrlEncodedParams.push_back(propertyValue.get<std::string>());
										}
									}
								}
							}

							reqModelMap.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
						}
						else
							continue;

						std::ostringstream requestPath;
						requestPath << "/" << modelControllerName << "/" << methodName;

						reqModels.insert(std::pair<std::string, std::map<std::string, request_model> >(requestPath.str(), reqModelMap));
					}
				}
			}

			return true;
		}

        bool init_controllers()
		{
			std::string path = webappPath + "/controllers/";

			DIR *currentDir = opendir(path.c_str());
			if (!currentDir)
				return false;

			std::list<std::string> entries;

			while (dirent *entry = readdir(currentDir))
			{
				std::ifstream isFile(path + entry->d_name);
				if (isFile.is_open())
					entries.push_back(entry->d_name);
			}
			closedir(currentDir);

			for (std::list<std::string>::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
			{
				const std::string controllerName = utils::split_string(*entry, '.').front();

				controllers.insert(controllerName);
			}

			return true;
		}

		bool init_pages()
		{
			std::string path = webappPath + "/views";

			std::map<std::string, std::string> pagesContent;

			std::string pageContent;
			get_masterpage(pageContent);

			DIR *currentDir = opendir(path.c_str());
			if (!currentDir)
				return false;

			std::list<std::string> entries;

			while (dirent *entry = readdir(currentDir))
			{
				if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;

				std::string controller = entry->d_name;
				std::string ctrlPath = path + "/" + controller;

				DIR *ctrlDir = opendir(ctrlPath.c_str());
				if (!ctrlDir)
					continue;

				while (dirent *ctrlEntry = readdir(ctrlDir))
				{
					if (ctrlEntry->d_type != DT_REG)
						continue;

					std::vector<std::string> splittedFileName = utils::split_string(ctrlEntry->d_name, '.');
					if (splittedFileName.size() == 2 && splittedFileName.back() == "html")
					{
						std::string pageUrl = std::string("/") + controller + "/" + splittedFileName[0];
						std::string viewFilePath = ctrlPath + "/" + ctrlEntry->d_name;

						std::ifstream viewFile(viewFilePath);
						if (!viewFile.is_open())
							continue;
						viewFile.seekg(0, std::ios::end);
						std::streampos fileSize = viewFile.tellg();
						viewFile.seekg(0, std::ios::beg);

						std::string viewFilePageContent;
						viewFilePageContent.resize((unsigned int)fileSize);
						viewFile.read(&viewFilePageContent[0], fileSize);


						std::map<std::string, std::string> pageBlocks;

						for (size_t blockPos = viewFilePageContent.find(WEBMVC_VIEWDATA, 0); blockPos != std::string::npos; blockPos = viewFilePageContent.find(WEBMVC_VIEWDATA, blockPos))
						{
							blockPos += std::string(WEBMVC_VIEWDATA).length();

							size_t endBlockNamePos = viewFilePageContent.find(WEBMVC_BLOCK_END, blockPos);
							if (blockPos == std::string::npos)
								break;

							std::string pageBlockName = viewFilePageContent.substr(blockPos, endBlockNamePos - blockPos);
							blockPos = endBlockNamePos + std::string(WEBMVC_BLOCK_END).length();

							size_t endBlockBlockPos = viewFilePageContent.find(WEBMVC_VIEWDATA_CLOSED, blockPos);
							std::string pageBlockContent = viewFilePageContent.substr(blockPos, endBlockBlockPos - blockPos);

							pageBlocks.insert(std::pair<std::string, std::string>(pageBlockName, pageBlockContent));

							blockPos = endBlockBlockPos + std::string(WEBMVC_CLOSEBLOCK_END).length();
						}

						std::string currentPageContent = pageContent;

						currentPageContent = utils::multiply_replace_string(currentPageContent, WEBMVC_VIEWDATA, std::string(WEBMVC_CLOSEBLOCK_END), pageBlocks);

						pagesContent.insert(std::pair<std::string, std::string>(pageUrl, currentPageContent));
					}

				}

				closedir(ctrlDir);
			}
			closedir(currentDir);

			this->pages = pagesContent;

			return true;
		}
	private:

    };
    
    class gset_start_application_handler
    {
        gset_start_application_handler();
    public:
        gset_start_application_handler(webmvcpp_start_application_fn fn)
        {
            webmvcpp::mvc_handlers::global()->startApplication = fn;
        }
    };

    class gset_stop_application_handler
    {
        gset_stop_application_handler();
    public:
        gset_stop_application_handler(webmvcpp_stop_application_fn fn)
        {
            webmvcpp::mvc_handlers::global()->stopApplication = fn;
        }
    };

    class gset_create_session_handler
    {
        gset_create_session_handler();
    public:
        gset_create_session_handler(webmvcpp_create_session_fn fn)
        {
            webmvcpp::mvc_handlers::global()->createSession = fn;
        }

    };

    class gset_remove_session_handler
    {
        gset_remove_session_handler();
    public:
        gset_remove_session_handler(webmvcpp_remove_session_fn fn)
        {
            webmvcpp::mvc_handlers::global()->removeSession = fn;
        }
    };

    class gadd_request_handler
    {
        gadd_request_handler();
    public:
        gadd_request_handler(const std::string & url, webmvcpp_request_handler fn)
        {
            webmvcpp::mvc_handlers::global()->requests.insert(std::pair<std::string, webmvcpp_request_handler>(url, fn));
        }
    };

    class gset_master_page_handler
    {
        gset_master_page_handler();
    public:
        gset_master_page_handler(webmvcpp_view_handler fn)
        {
            webmvcpp::mvc_handlers::global()->masterPageHandler = fn;
        }
    };

    class gset_check_authorized_handler
    {
        gset_check_authorized_handler();
    public:
        gset_check_authorized_handler(webmvcpp_check_authorized_fn fn)
        {
            webmvcpp::mvc_handlers::global()->checkAuthorized = fn;
        }
    };

    class gadd_view_handler
    {
        gadd_view_handler();
    public:
        gadd_view_handler(const std::string & url, webmvcpp_view_handler fn)\
        {
            webmvcpp::mvc_handlers::global()->views.insert(std::pair<std::string, webmvcpp_view_handler>(url, fn));
        }
    };
}

#endif // WEBMVCPP_APPLICATION_MODULE_H