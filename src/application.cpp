#include "webmvcpp.h" 
#include "application.h"
#include "handlers.h"

const std::string g_blockWebmvcppViewData = "<webmvcpp:content:";
const std::string g_blockWebmvcppViewDataClosed = "</webmvcpp:content:";
const std::string g_closedBlockEnd = " />";
const std::string g_blockEnd = ">";


using namespace nlohmann;

namespace webmvcpp
{
    mvc_handlers *mvc_handlers::g = NULL;

    application::application()
    {

    }

    application::~application()
    {
    }

    void application::start()
    {

    }
    void application::stop()
    {

    }

    bool application::init(const std::string & w, const std::string & s)
    {
        webappPath = w;
        staticPath = s;

        this->init_models();
        this->init_controllers();
        this->init_pages();

        return true;
    }

    void application::add_route(const std::string & path, const std::string & newPath)
    {
        routeMap.insert(std::pair<std::string, std::string>(path, newPath));
    }

    bool application::redirect_to(http_response & response, const std::string & url)
    {
        response.status = "302 found";
        response.header.insert(std::pair<std::string, std::string>("Location", url));

        return true;
    }

    bool application::get_masterpage(std::string & pageFile)
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

    bool application::init_pages()
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

				std::vector<std::string> splittedFileName = split_string(ctrlEntry->d_name, '.');
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

					for (size_t blockPos = viewFilePageContent.find(g_blockWebmvcppViewData, 0); blockPos != std::string::npos; blockPos = viewFilePageContent.find(g_blockWebmvcppViewData, blockPos))
					{
						blockPos += g_blockWebmvcppViewData.length();

						size_t endBlockNamePos = viewFilePageContent.find(g_blockEnd, blockPos);
						if (blockPos == std::string::npos)
							break;

						std::string pageBlockName = viewFilePageContent.substr(blockPos, endBlockNamePos - blockPos);
						blockPos = endBlockNamePos + g_blockEnd.length();

						size_t endBlockBlockPos = viewFilePageContent.find(g_blockWebmvcppViewDataClosed, blockPos);					
						std::string pageBlockContent = viewFilePageContent.substr(blockPos, endBlockBlockPos - blockPos);

						pageBlocks.insert(std::pair<std::string, std::string>(pageBlockName, pageBlockContent));

						blockPos = endBlockBlockPos + g_blockWebmvcppViewDataClosed.length();
					}

					std::string currentPageContent = pageContent;

					currentPageContent = multiply_replace_string(currentPageContent, g_blockWebmvcppViewData, g_closedBlockEnd, pageBlocks);

					pagesContent.insert(std::pair<std::string, std::string>(pageUrl, currentPageContent));
				}
					
			}

			closedir(ctrlDir);
		}
		closedir(currentDir);

		this->pages = pagesContent;

        return true;
    }

    bool application::init_models()
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

                        const std::string & reModelIt = it.key();
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

						//reqModel.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
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

                        //reqModelMap.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
                    }
                    else
                        continue;

                    std::ostringstream requestPath;
                    requestPath << "/" << modelControllerName << "/" << methodName;

                    //reqModels.insert(std::pair<std::string, std::map<std::string, request_model> >(requestPath.str(), reqModelMap));
                }
            }
        }

		return true;
    }

	bool application::init_controllers()
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
            const std::string controllerName = split_string(*entry, '.').front();

            controllers.insert(controllerName);
        }

		return true;
    }
}