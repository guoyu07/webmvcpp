#ifndef WEBMVCPP_BUILDER_H
#define WEBMVCPP_BUILDER_H

namespace webmvcpp
{
    class builder
    {
    public:
        builder(){}

		static std::string generateApplicationMainContent(const char* applicationName)
		{
			std::ostringstream mainContent;
			mainContent << "#ifdef _WIN32" << std::endl;
			mainContent << "#include <windows.h>" << std::endl;
			mainContent << "BOOL __stdcall DllMain(HINSTANCE hinstance, DWORD reason, LPVOID reserved)" << std::endl;
			mainContent << "{" << std::endl;
			mainContent << "    return TRUE;" << std::endl;
			mainContent << "}" << std::endl;
			mainContent << "#endif" << std::endl;
			mainContent << "#include \"" << applicationName << ".h\"" << std::endl;
			mainContent << std::endl;
			mainContent << "webmvcpp::application* Application = NULL;" << std::endl;
			mainContent << std::endl;
			mainContent << "namespace webmvcpp" << std::endl;
			mainContent << "{" << std::endl;
			mainContent << "	mvc_handlers *mvc_handlers::g = NULL;" << std::endl;
			mainContent << "}" << std::endl;
			mainContent << "" << std::endl;
			mainContent << "extern \"C\"" << std::endl;
			mainContent << "#ifdef _WIN32" << std::endl;
			mainContent << "    __declspec(dllexport)" << std::endl;
			mainContent << "#endif" << std::endl;
			mainContent << "bool init_module(const char *webAppPath, const char *staticPath)" << std::endl;
			mainContent << "{" << std::endl;
			mainContent << "	std::string w = webAppPath;" << std::endl;
			mainContent << "	std::string s = staticPath;" << std::endl;
			mainContent << std::endl;
			mainContent << "    Application = new webmvcpp::" << applicationName << "();" << std::endl;
			mainContent << "    if (!Application->init(webAppPath, staticPath))" << std::endl;
			mainContent << "        return false;    " << std::endl;
			mainContent << std::endl;
			mainContent << "    return true;" << std::endl;
			mainContent << "}" << std::endl;
			mainContent << std::endl;
			mainContent << "extern \"C\"" << std::endl;
			mainContent << "#ifdef _WIN32" << std::endl;
			mainContent << "__declspec(dllexport)" << std::endl;
			mainContent << "#endif" << std::endl;
			mainContent << "void *webmvcpp_instance()" << std::endl;
			mainContent << "{" << std::endl;
			mainContent << "    return Application;" << std::endl;
			mainContent << "}" << std::endl;
			mainContent << std::endl;
			mainContent << "extern \"C\"" << std::endl;
			mainContent << "#ifdef _WIN32" << std::endl;
			mainContent << "    __declspec(dllexport)" << std::endl;
			mainContent << "#endif" << std::endl;
			mainContent << "void deinit_module()" << std::endl;
			mainContent << "{" << std::endl;
			mainContent << "    delete Application;" << std::endl;
			mainContent << "}" << std::endl;

			return mainContent.str();
		}

		static std::string generateApplicationDef(const char* applicationName)
		{
			std::ostringstream defContent;

			defContent << "LIBRARY \"" << applicationName << "\"" << std::endl;
			defContent << std::endl;
			defContent << "	EXPORTS" << std::endl;
			defContent << "	webmvcpp_instance" << std::endl;
			defContent << "	init_module" << std::endl;
			defContent << "	deinit_module" << std::endl;

			return defContent.str();
		}

		bool compile(const std::string & sourceFile, std::string & objFile, std::string & result)
		{
			return true;
		}

		bool linkApplication(const std::list<std::string> & objFiles, std::string & appFile, std::string & result)
		{
			return true;
		}
    };
}

#endif // WEBMVCPP_BUILDER_H
