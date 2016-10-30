#ifndef WEBMVCPP_APPLICATION_BUILDER_H
#define WEBMVCPP_APPLICATION_BUILDER_H

extern "C" const char webmvcpp_headers_hpp_amalgamation[];

namespace webmvcpp
{
    struct view_control {
        std::string content;
        std::vector<std::string> parameters;
    };

    class webapplication_builder
    {
        webapplication_builder();
    public:
        webapplication_builder(const std::string & appName, const std::string & webAppPath) :
            applicationName(appName),
            webApplicationPath(webAppPath),
            headersHppAmalgamation(webAppPath + "/webmvcpp.hpp")
        {
            std::ofstream outputHeadersHppFile(headersHppAmalgamation, std::ofstream::out);
            outputHeadersHppFile << webmvcpp_headers_hpp_amalgamation;
            outputHeadersHppFile.close();
        }

        virtual ~webapplication_builder()
        {
#if defined (_WIN32)
            ::DeleteFileA(headersHppAmalgamation.c_str());
#else
            remove(headersHppAmalgamation.c_str());
#endif
        }
  
        bool generateModels(const std::string & outputFile)
        {
            std::ofstream modelsContent(outputFile, std::ofstream::out);
            modelsContent << "#include \"" << applicationName << ".h\"" << std::endl;
            modelsContent << "namespace webmvcpp" << std::endl;
            modelsContent << "{" << std::endl << std::endl;
            modelsContent << std::endl;

            std::string path = webApplicationPath + "/models";
            DIR *currentDir = opendir(path.c_str());
            if (!currentDir)
                return false;

            while (dirent *entry = readdir(currentDir))
            {
                if (entry->d_type == DT_REG)
                {
                    std::string filePath = path + "/" + entry->d_name;

                    std::ifstream modelFile(filePath);
                    if (!modelFile.is_open())
                        return false;

                    modelsContent << "const char json_" << entry->d_name << "[] = \\" << std::endl;

                    std::string jsonLine;
                    while (std::getline(modelFile, jsonLine)) {
                        if (jsonLine.length() != 0)
                            modelsContent << "    \"" << utils::replace_string(jsonLine, "\"", "\\\"") << "\" \\" << std::endl;
                    }

                    modelsContent << ";" << std::endl;

                    modelsContent << "const json mdl_" << entry->d_name << " = json::parse(" << "json_" << entry->d_name << ");" << std::endl;
                    modelsContent << "gadd_request_model reqModel_" << entry->d_name << "(\"" << entry->d_name << "\", mdl_" << entry->d_name << ");" << std::endl;
                }
            }
            closedir(currentDir);

            modelsContent << std::endl << std::endl << "}";
            modelsContent.close();

            return true;
        }

        bool generateControllers(const std::string & outputFile)
        {
            std::ofstream controllersContent(outputFile, std::ofstream::out);
            controllersContent << "#include \"" << applicationName << ".h\"" << std::endl;
            controllersContent << "namespace webmvcpp" << std::endl;
            controllersContent << "{" << std::endl << std::endl;
            controllersContent << std::endl;

            std::string path = webApplicationPath + "/controllers/";

            DIR *currentDir = opendir(path.c_str());
            if (!currentDir)
                return false;

            while (dirent *entry = readdir(currentDir))
            {
                if (entry->d_type == DT_REG) {
                    std::string fileName = entry->d_name;

                    const std::vector<std::string> splittedFile = utils::split_string(fileName, '.');

                    if (splittedFile.size() == 2 && splittedFile.back() == "cpp")
                        controllersContent << "gadd_controller controller_" << splittedFile.front() << "(\"" << splittedFile.front() << "\");" << std::endl;
                }
            }
            closedir(currentDir);

            controllersContent << std::endl << std::endl << "}";
            controllersContent.close();

            return true;
        }

        bool generateViews(const std::string & outputFile)
        {
            std::ofstream viewsContent(outputFile, std::ofstream::out);
            viewsContent << "#include \"" << applicationName << ".h\"" << std::endl;
            viewsContent << "namespace webmvcpp" << std::endl;
            viewsContent << "{" << std::endl << std::endl;
            viewsContent << std::endl;

            std::string viewsPath = webApplicationPath + "/views";

            std::string mainContent;
            std::ifstream mainPageFile(viewsPath + "/main.html");
            if (!mainPageFile.is_open())
                return false;

            mainPageFile.seekg(0, std::ios::end);
            std::streampos fileSize = mainPageFile.tellg();
            mainPageFile.seekg(0, std::ios::beg);

            mainContent.resize((unsigned int)fileSize);
            mainPageFile.read(&mainContent[0], fileSize);

            DIR *cntrlsDir = opendir(viewsPath.c_str());
            if (!cntrlsDir)
                return false;

            std::map<std::string, view_control> controls;

            while (dirent *entry = readdir(cntrlsDir))
            {
                if (entry->d_type == DT_DIR)
                    continue;
                std::string fileName = entry->d_name;
                std::vector<std::string> splittedName = utils::split_string(fileName, '.');
                if ((splittedName.size() < 2) || (splittedName[splittedName.size() - 1] != "ctrl"))
                    continue;
                std::string filePath = viewsPath + "/" + fileName;
                std::ifstream ctrlFile(filePath);
                if (!ctrlFile.is_open())
                    continue;

                std::string controlTextFileContent;

                std::string ctrlLine;
                while (std::getline(ctrlFile, ctrlLine)) {
                    controlTextFileContent += ctrlLine;
                }
                std::map<std::string, std::string> CntrlCodeBlocks;

                size_t controlBlockPos = controlTextFileContent.find(WEBMVC_VIEWCONTROL, 0);
                size_t controlEndBlockPos = controlTextFileContent.rfind(WEBMVC_VIEWCONTROL_CLOSED, controlTextFileContent.length() - 1);
                if (controlBlockPos == std::string::npos || controlEndBlockPos == std::string::npos)
                    continue;
                size_t controlParametersPos = controlBlockPos + std::string(WEBMVC_VIEWCONTROL).length();
                size_t controlParametersEndPos = controlTextFileContent.find(WEBMVC_BLOCK_END, controlParametersPos);
                if (controlParametersEndPos == std::string::npos)
                    continue;

                std::string parameters = controlTextFileContent.substr(controlParametersPos, controlParametersEndPos - controlParametersPos);
                if (parameters.length() == 0)
                    continue;

                size_t controlContentPos = controlParametersEndPos + 1; // sizeof WEBMVC_BLOCK_END

                view_control control;
                control.content = controlTextFileContent.substr(controlContentPos, controlEndBlockPos - controlContentPos);
                control.parameters = utils::split_string(parameters, ':');
                std::string controlname = control.parameters[0];
                control.parameters.erase(control.parameters.begin());
                controls.insert(std::pair<std::string, view_control>(controlname, control));
            }
            closedir(cntrlsDir);

            DIR *cntrlrsDir = opendir(viewsPath.c_str());
            if (!cntrlrsDir)
                return false;

            std::list<std::string> entries;

            while (dirent *entry = readdir(cntrlrsDir))
            {
                if (entry->d_type != DT_DIR || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    continue;

                std::string controller = entry->d_name;
                std::string ctrlPath = viewsPath + "/" + controller;

                std::string controllerContent = mainContent;

                std::ifstream cntrlrContentPageFile(viewsPath + "/main.html");
                if (cntrlrContentPageFile.is_open())
                {
                    cntrlrContentPageFile.seekg(0, std::ios::end);
                    std::streampos fileSize = cntrlrContentPageFile.tellg();
                    cntrlrContentPageFile.seekg(0, std::ios::beg);

                    controllerContent.resize((unsigned int)fileSize);
                    cntrlrContentPageFile.read(&controllerContent[0], fileSize);
                }

                DIR *cntrlDir = opendir(ctrlPath.c_str());
                if (!cntrlDir)
                    continue;

                while (dirent *ctrlEntry = readdir(cntrlDir))
                {
                    if (ctrlEntry->d_type != DT_REG)
                        continue;

                    std::vector<std::string> splittedFileName = utils::split_string(ctrlEntry->d_name, '.');
                    if (splittedFileName.size() == 2 && splittedFileName.back() == "html")
                    {
                        std::string page = splittedFileName[0];
                        std::string pageUrl = std::string("/") + controller + "/" + page;
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

                        std::string currentPageContent = utils::multiply_replace_string(controllerContent, WEBMVC_VIEWDATA, std::string(WEBMVC_CLOSEBLOCK_END), pageBlocks);

                        std::string pageContent;
                        {
                            size_t prevPos = 0;
                            size_t curPos = 0;
                            while ((curPos = currentPageContent.find(WEBMVC_VIEWCONTROL, curPos)) != std::string::npos)
                            {
                                pageContent += currentPageContent.substr(prevPos, curPos - prevPos);
                                curPos += std::string(WEBMVC_VIEWCONTROL).length();

                                size_t blockEnd = currentPageContent.find(WEBMVC_CLOSEBLOCK_END, curPos);
                                if (blockEnd == std::string::npos)
                                    break;
                                std::string ctrlParamsStr = currentPageContent.substr(curPos, blockEnd - curPos);
                                std::vector<std::string> ctrlParams = utils::split_string(ctrlParamsStr, ':');
                                if (ctrlParams.size() == 0)
                                    break;

                                std::string ctrlName = ctrlParams[0];
                                ctrlParams.erase(ctrlParams.begin());
                                std::map<std::string, view_control>::iterator ctrlIt = controls.find(ctrlName);
                                if (ctrlIt == controls.end() || ctrlIt->second.parameters.size() != ctrlParams.size())
                                    break;

                                if (ctrlIt->second.parameters.size() == 0)
                                {
                                    pageContent += ctrlIt->second.content;
                                }
                                else
                                {
                                    std::ostringstream ctrlCode;
                                    ctrlCode << "{%{" << std::endl;
                                    for (unsigned int i = 0; i < ctrlIt->second.parameters.size(); ++i)
                                    {
                                        ctrlCode << "    std::string " << ctrlIt->second.parameters[i] << " = \"" << ctrlParams[i] << "\";" << std::endl;
                                    }
                                    ctrlCode << "%}" << std::endl;
                                    ctrlCode << ctrlIt->second.content << std::endl;
                                    ctrlCode << "{%" << std::endl;
                                    ctrlCode << "}%}" << std::endl;
                                    pageContent += ctrlCode.str();
                                }

                                curPos = blockEnd + std::string(WEBMVC_CLOSEBLOCK_END).length();
                                prevPos = curPos;
                            }
                            size_t contentLength = currentPageContent.length();
                            if (prevPos != contentLength) {
                                pageContent += currentPageContent.substr(prevPos, contentLength - prevPos);
                            }
                        }



                        viewsContent << std::endl << "view_handler(" + controller + ", " + page + ", [](const http_request & request, variant_map & session, mvc_view_data & viewData) -> std::string {" << std::endl;

                        viewsContent << "    std::ostringstream pageContent;" << std::endl;

                        pageContent = utils::multiply_replace_string(pageContent, "{$", "}", "{%= viewData[\"", "\"] %}");

                        size_t htmlBlockBegin = 0;

                        for (size_t codeBlockPos = pageContent.find("{%", 0); codeBlockPos != std::string::npos; codeBlockPos = pageContent.find("{%", codeBlockPos))
                        {
                            codeBlockPos += 2; // "{%"

                            size_t endCodeBlockPos = pageContent.find("%}", codeBlockPos);
                            if (endCodeBlockPos == std::string::npos)
                                break;

                            std::string codeBlock = pageContent.substr(codeBlockPos, endCodeBlockPos - codeBlockPos);

                            codeBlockPos -= 2; // "{%"
                            endCodeBlockPos += 2; //"%}"                        

                            viewsContent << "pageContent << " + utils::to_cHexString(pageContent.substr(htmlBlockBegin, codeBlockPos - htmlBlockBegin)) + "; " << std::endl;

                            codeBlockPos = endCodeBlockPos;
                            htmlBlockBegin = endCodeBlockPos; //"%}"

                            if (codeBlock.length() > 0 && codeBlock[0] == '=')
                                viewsContent << "pageContent << " + codeBlock.substr(1) + ";" << std::endl;
                            else
                                viewsContent << std::endl << codeBlock << ";" << std::endl;
                        }

                        if (htmlBlockBegin != pageContent.length())
                            viewsContent << "pageContent << " + utils::to_cHexString(pageContent.substr(htmlBlockBegin)) + "; " << std::endl;

                        viewsContent << "    return pageContent.str();" << std::endl;
                        viewsContent << "});" << std::endl;
                    }
                }

                closedir(cntrlDir);
            }
            closedir(cntrlrsDir);

            viewsContent << std::endl << std::endl << "}";
            viewsContent.close();

            return true;
        }

        bool compile(const std::string & sourceFile, const std::list<std::string> & definitions, const std::list<std::string> & includeDirectories, std::string & objFile,  std::string & result)
        {
            std::vector<std::string> splittedFile = utils::split_string(sourceFile, '.');
            if (splittedFile.size() < 2)
                return false;

            std::string fileExtension = splittedFile[splittedFile.size() - 1];

            std::ostringstream cmd;

            if (fileExtension == "cpp")
                cmd << WEBMVCPP_COMPILER_CPP << " " << compiler_cpp_flags;
            else if (fileExtension == "c")
                cmd << WEBMVCPP_COMPILER_C << " " << compiler_c_flags;
            else
                return false;

#if defined (_WIN32)
            for (std::list<std::string>::const_iterator it = definitions.begin(); it != definitions.end(); ++it) {
                cmd << " /D" << *it;
            }
            for (std::list<std::string>::const_iterator it = includeDirectories.begin(); it != includeDirectories.end(); ++it) {
                cmd << " /I\"" << *it << "\"";
            }
            cmd << " /I\"" << webApplicationPath << "\" \"" << sourceFile << "\" " << "/Fo\"" << objFile << "\"";
#else
            for (std::list<std::string>::const_iterator it = definitions.begin(); it != definitions.end(); ++it) {
                cmd << " -D" << *it;
        }
            for (std::list<std::string>::const_iterator it = includeDirectories.begin(); it != includeDirectories.end(); ++it) {
                cmd << " -I\"" << *it << "\"";
            }
            cmd << " -I\"" << webApplicationPath << "\" \"" << sourceFile << "\" " << "-o\"" << objFile << "\"";
#endif
            printf("%s\n", cmd.str().c_str());

            char buf[1024];
#if defined (_WIN32)
            FILE *fp = _popen(cmd.str().c_str(), "r");
#else
            FILE *fp = popen(cmd.str().c_str(), "r");
#endif
            if (fp == NULL) {
                printf("Error opening pipe!\n");
                return false;
            }

            while (fgets(buf, sizeof(buf), fp) != NULL) {
                printf("%s\n", buf);
            }

#if defined (_WIN32)
            if (_pclose(fp))
#else
            if (pclose(fp))
#endif
            {
                return false;
            }

            return true;
        }

#if defined (_WIN32)
        bool linkApplication(const std::list<std::string> & objFiles, const std::list<std::string> & linkLinbraries, std::string & defFile, std::string & appFile, std::string & result)
#else
        bool linkApplication(const std::list<std::string> & objFiles, const std::list<std::string> & linkLinbraries, std::string & appFile, std::string & result)
#endif
        {
            std::ostringstream cmd;
            cmd << WEBMVCPP_LINKER << " " << linker_flags;

            for (std::list<std::string>::const_iterator it = objFiles.begin(); it != objFiles.end(); ++it)
            {
                cmd << " \"" << *it << "\"";
            }

#if defined (_WIN32)
            cmd << " /DEF:\"" << defFile << "\"";
#endif

#if defined (_WIN32)
            for (std::list<std::string>::const_iterator it = linkLinbraries.begin(); it != linkLinbraries.end(); ++it)
            {
                cmd << " \"" << *it << "\"";
            }
#else
            for (std::list<std::string>::const_iterator it = linkLinbraries.begin(); it != linkLinbraries.end(); ++it)
            {
                cmd << "-l\"" << *it << "\"";
            }
#endif

#if defined (_WIN32)
            cmd << " /OUT:\"" << appFile << "\"";
#else
            cmd << " -o \"" << appFile << "\"";
#endif

            printf("%s\n", cmd.str().c_str());

            char buf[1024];
#if defined (_WIN32)
            FILE *fp = _popen(cmd.str().c_str(), "r");
#else
            FILE *fp = popen(cmd.str().c_str(), "r");
#endif

            if (fp == NULL) {
                printf("Error opening pipe!\n");
                return false;
            }

            while (fgets(buf, sizeof(buf), fp) != NULL) {
                printf("%s\n", buf);
            }

#if defined (_WIN32)
            if (_pclose(fp))
#else
            if (pclose(fp))
#endif
            {
                return false;
            }

            return true;
        }

    private:
        std::string applicationName;
        std::string webApplicationPath;
        std::string headersHppAmalgamation;

#if defined (_WIN32)
        const char *compiler_c_flags = "/c /Ox /nologo";
        const char *compiler_cpp_flags = "/c /EHsc /Ox /nologo";
        const char *linker_flags = "/DLL /SUBSYSTEM:CONSOLE /nologo";
#elif (__APPLE__)
        const char *compiler_c_flags = "-O3 -fPIC -Wall -Ofast -fno-rtti -c";
        const char *compiler_cpp_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti -c";
        const char *linker_flags = "-ldl -pthread -shared";
#else
        const char *compiler_c_flags = "-O3 -fPIC -Wall -Ofast -fno-rtti  -c";
        const char *compiler_cpp_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti  -c";
        const char *linker_flags = "-ldl -pthread -shared";
#endif


    };
}

#endif // WEBMVCPP_APPLICATION_BUILDER_H