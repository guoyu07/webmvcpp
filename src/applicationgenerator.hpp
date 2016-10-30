#ifndef WEBMVCPP_APPLICATION_GENERATOR_H
#define WEBMVCPP_APPLICATION_GENERATOR_H

extern "C" const char webmvcpp_headers_hpp_amalgamation[];

namespace webmvcpp
{
    class webapplication_generator
    {
        webapplication_generator();
    public:
        webapplication_generator(const std::string & appName) :
        applicationName(appName)
        {
            std::vector<std::string> splittedDomain = utils::split_string(appName, '.');
            for (std::vector<std::string>::iterator it = splittedDomain.begin(); it != splittedDomain.end(); ++it)
            {
                if (it->length() == 0)
                    continue;

                std::string lowerDomainPart = utils::to_lower(*it);
                std::string upperDomainPart = utils::to_upper(*it);
                char firstChar = upperDomainPart[0];

                applicationClassName += std::string(&firstChar, 1);
                if (lowerDomainPart.length() > 1)
                    applicationClassName += lowerDomainPart.substr(1);
            }
        }

        virtual ~webapplication_generator()
        {
        }

        void generate_webmvcpp_application(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "#ifdef _WIN32" << std::endl;
            ofs << "#include <windows.h>" << std::endl;
            ofs << "BOOL __stdcall DllMain(HINSTANCE hinstance, DWORD reason, LPVOID reserved)" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    return TRUE;" << std::endl;
            ofs << "}" << std::endl;
            ofs << "#endif" << std::endl;
            ofs << "#include \"" << applicationName << ".h\"" << std::endl;
            ofs << std::endl;
            ofs << "webmvcpp::webapplication* Application = NULL;" << std::endl;
            ofs << std::endl;
            ofs << "namespace webmvcpp" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    mvc_handlers *mvc_handlers::g = NULL;" << std::endl;
            ofs << "}" << std::endl;
            ofs << "" << std::endl;
            ofs << "extern \"C\"" << std::endl;
            ofs << "#ifdef _WIN32" << std::endl;
            ofs << "    __declspec(dllexport)" << std::endl;
            ofs << "#endif" << std::endl;
            ofs << "bool init_module(const char *webAppPath, const char *staticPath)" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    std::string w = webAppPath;" << std::endl;
            ofs << "    std::string s = staticPath;" << std::endl;
            ofs << std::endl;
            ofs << "    Application = new webmvcpp::" << applicationClassName << "();" << std::endl;
            ofs << "    if (!Application->init(webAppPath, staticPath))" << std::endl;
            ofs << "        return false;    " << std::endl;
            ofs << std::endl;
            ofs << "    return true;" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
            ofs << "extern \"C\"" << std::endl;
            ofs << "#ifdef _WIN32" << std::endl;
            ofs << "__declspec(dllexport)" << std::endl;
            ofs << "#endif" << std::endl;
            ofs << "void *webmvcpp_instance()" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    return Application;" << std::endl;
            ofs << "}" << std::endl;
            ofs << std::endl;
            ofs << "extern \"C\"" << std::endl;
            ofs << "#ifdef _WIN32" << std::endl;
            ofs << "    __declspec(dllexport)" << std::endl;
            ofs << "#endif" << std::endl;
            ofs << "void deinit_module()" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    delete Application;" << std::endl;
            ofs << "}" << std::endl;

            ofs.close();
        }

        void generate_application_config(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "{" << std::endl;
            ofs << "  \"projectName\": \"sample\"," << std::endl;
            ofs << "  \"bindPort\": 8081," << std::endl;
            ofs << "  \"maxFormContentSize\": 16384," << std::endl;
            ofs << "  \"hosts\": {" << std::endl;
            ofs << "      \"" << applicationName << "\": {" << std::endl;
            ofs << "      \"alias\": [\"" << applicationName << "\", \"localhost\"]," << std::endl;
            ofs << "          \"sessionsEnabled\": true," << std::endl;
            ofs << "          \"sessionTimeout\": 24," << std::endl;
            ofs << "          \"module\": \"\"," << std::endl;
            ofs << "          \"staticPath\": \"./static\"," << std::endl;
            ofs << "          \"webappPath\": \"./webapp\"" << std::endl;
            ofs << "      }" << std::endl;
            ofs << "   }" << std::endl;
            ofs << "}" << std::endl;
            ofs.close();
        }

        void generate_application_header(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "#ifndef WEBMVCPP_APPLICATION_HEADER_H" << std::endl;
            ofs << "#define WEBMVCPP_APPLICATION_HEADER_H" << std::endl;
            ofs << std::endl;
            ofs << "#include \"webmvcpp.hpp\"" << std::endl;
            ofs << std::endl;
            ofs << "namespace webmvcpp" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    class " << applicationClassName << " : public webapplication" << std::endl;
            ofs << "    {" << std::endl;
            ofs << "    public:" << std::endl;
            ofs << "        virtual bool init(const std::string & w, const std::string & s)" << std::endl;
            ofs << "        {" << std::endl;
            ofs << "            add_route(\"/\", \"/main/index\");" << std::endl;
            ofs << "            return webapplication::init(w, s);" << std::endl;
            ofs << "        }" << std::endl;
            ofs << std::endl;
            ofs << "    protected:" << std::endl;
            ofs << std::endl;
            ofs << "    private:" << std::endl;
            ofs << std::endl;
            ofs << "    };" << std::endl;
            ofs << std::endl;
            ofs << "    extern " << applicationClassName << " *Application;" << std::endl;
            ofs << "}" << std::endl;
            ofs << "#endif // WEBMVCPP_APPLICATION_HEADER_H" << std::endl;
            ofs.close();
        }

        void generate_application_def(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);

            ofs << "LIBRARY \"" << applicationName << ".dll\"" << std::endl;
            ofs << std::endl;
            ofs << "    EXPORTS" << std::endl;
            ofs << "    webmvcpp_instance" << std::endl;
            ofs << "    init_module" << std::endl;
            ofs << "    deinit_module" << std::endl;

            ofs.close();
        }

        void generate_model(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "{" << std::endl;
            ofs << "    \"index\": {" << std::endl;
            ofs << "        \"GET\": {" << std::endl;
            ofs << "        }" << std::endl;
            ofs << "    }" << std::endl;
            ofs << "}" << std::endl;
            ofs.close();
        }

        void generate_controller(const std::string & outputFile, const std::string & controllerName)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "#include \"" << applicationName << "\"" << std::endl;
            ofs << std::endl;
            ofs << "namespace webmvcpp" << std::endl;
            ofs << "{" << std::endl;
            ofs << "    controller_requests_handler(" << controllerName << ", [](const http_request & request, http_response & response, variant_map & session) -> bool {" << std::endl;
            ofs << "        return true;" << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    request_handler(" << controllerName << ", index, [](const http_request & request, http_response & response, variant_map & session, mvc_view_data & viewData) -> bool {" << std::endl;
            ofs << "        return true;" << std::endl;
            ofs << "    });" << std::endl;
            ofs << "}" << std::endl;
            ofs.close();
        }

        void generate_view(const std::string & outputFile, const std::string & viewName)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "<webmvcpp:content:title>" << viewName << " title</webmvcpp:content:title>" << std::endl;
            ofs << "<webmvcpp:content:main>" << viewName << " content</webmvcpp:content:title>" << std::endl;
            ofs.close();
        }

        void generate_masterpage(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "<!DOCTYPE html>" << std::endl;
            ofs << "<html lang = \"en\">" << std::endl;
            ofs << "<head>" << std::endl;
            ofs << "<meta charset = \"utf-8\">" << std::endl;
            ofs << "<meta http - equiv = \"X-UA-Compatible\" content = \"IE=edge\">" << std::endl;
            ofs << "<meta name = \"viewport\" content = \"width=device-width, initial-scale=1\">" << std::endl;
            ofs << "<meta name = \"description\" content = \"" << applicationName << "\">" << std::endl;
            ofs << "<title><webmvcpp:content:title /></title>" << std::endl;
            ofs << "</head>" << std::endl;
            ofs << "<body>" << std::endl;
            ofs << "<h2>{message}</h2>" << std::endl;
            ofs << "<webmvc:control:url />" << std::endl;
            ofs << "<webmvcpp:content:main />" << std::endl;
            ofs << "</body>" << std::endl;
            ofs << "</html>" << std::endl;
            ofs.close();
        }

        void generate_control(const std::string & outputFile, const std::string & ctrlName)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "<webmvc:control:" << ctrlName << ">" << std::endl;
            ofs << "        " << applicationName << ":" << std::endl;
            ofs << "</webmvc:control:" << ctrlName << ">" << std::endl;
            ofs.close();
        }

        void generate_webapp(const std::string & outputFile)
        {
            std::ofstream ofs(outputFile, std::ofstream::out);
            ofs << "#include \"" << applicationName << "\"" << std::endl;
            ofs << "    namespace webmvcpp" << std::endl;
            ofs << "    {" << std::endl;
            ofs << "    set_create_session_handler([](const http_request & request, variant_map & session) -> bool {" << std::endl;
            ofs << "        return true;" << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    set_remove_session_handler([](variant_map & session) -> void {" << std::endl;
            ofs << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    check_authorization_handler([](http_request & request, variant_map & session) -> bool {" << std::endl;
            ofs << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    set_masterpage_handler([](http_connection *connection, http_request & request, http_response & response, variant_map & session, mvc_view_data & viewData) -> bool {" << std::endl;
            ofs << "        return true;" << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    set_start_application_handler([]() -> bool {" << std::endl;
            ofs << "        return true;" << std::endl;
            ofs << "    });" << std::endl;
            ofs << std::endl;
            ofs << "    set_stop_application_handler([]() -> void {" << std::endl;
            ofs << std::endl;
            ofs << "    });" << std::endl;
            ofs << "}" << std::endl;
            ofs.close();
        }

        void generate_service(const std::string & outputFile)
        {

        }

        bool create_application()
        {
            std::list<std::string> directories;
            directories.push_back("./static");
            directories.push_back("./webapp");
            directories.push_back("./webapp/models");
            directories.push_back("./webapp/controllers");
            directories.push_back("./webapp/views");
            directories.push_back("./webapp/views/main");

            for (std::list<std::string>::iterator it = directories.begin(); it != directories.end(); ++it)
            {
#if defined (_WIN32)
                ::CreateDirectoryA(it->c_str(), NULL);
#else
                ::mkdir(it->c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
            }
            std::string defFile = "./webapp/" + applicationName + ".def";
            generate_application_def(defFile);
            generate_webmvcpp_application("./webapp/webmvcpp_application.cpp");
            generate_webapp("./webapp/webapp.cpp");
            generate_application_header("./webapp/" + applicationName + ".h");
            generate_control("./webapp/views/title.ctrl", "title");
            generate_masterpage("./webapp/views/main.html");
            generate_view("./webapp/views/main/index.html", "index");
            generate_controller("./webapp/controlles/main.cpp", "index");
            generate_model("./webapp/models/main");
            generate_application_config("./" + applicationName + ".webmvcpp");

            return true;
        }

    private:
        std::string applicationName;
        std::string applicationClassName;
        std::string webApplicationPath;
        std::string headersHppAmalgamation;
    };
}

#endif // WEBMVCPP_APPLICATION_GENERATOR_H