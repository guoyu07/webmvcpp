#ifndef WEBMVCPP_BUILDER_H
#define WEBMVCPP_BUILDER_H

namespace webmvcpp
{
    class builder
    {
    public:
        builder(const std::string & appName, const std::string & webAppPath):
		applicationName(appName),
		webApplicationPath(webAppPath)
		{}

		std::string applicationName;
		std::string webApplicationPath;

		std::string generateApplicationMainContent()
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

		void generateApplicationDef(const std::string & outputFile)
		{
			std::ofstream ofs(outputFile, std::ofstream::out);

			ofs << "LIBRARY \"" << applicationName << "\"" << std::endl;
			ofs << std::endl;
			ofs << "	EXPORTS" << std::endl;
			ofs << "	webmvcpp_instance" << std::endl;
			ofs << "	init_module" << std::endl;
			ofs << "	deinit_module" << std::endl;

			ofs.close();
		}

		bool compile(const std::string & sourceFile, std::string & objFile, std::string & result)
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
			cmd << " /I\"" << webApplicationPath << "\" \"" << sourceFile << "\" " << "/Fo\"" << objFile << "\"";
#else
			cmd << " /I\"" << webApplicationPath << "\" \"" << sourceFile << "\" " << "-o\"" << objFile << "\"";
#endif
			printf("%s", cmd.str().c_str());

			char buf[1024];
			FILE *fp = _popen(cmd.str().c_str(), "r");

			if (fp == NULL) {
				printf("Error opening pipe!\n");
				return false;
			}

			while (fgets(buf, sizeof(buf), fp) != NULL) {
				printf(" %s", buf);
			}

			if (_pclose(fp)) {
				return false;
			}

			return true;
		}

#if defined (_WIN32)
		bool linkApplication(const std::list<std::string> & objFiles, std::string & defFile, std::string & appFile, std::string & result)
#else
		bool linkApplication(const std::list<std::string> & objFiles, std::string & appFile, std::string & result)
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
			cmd << " /Fe\"" << appFile << "\"";
#else
			cmd << " -o \"" << appFile << "\"";
#endif

			printf("%s", cmd.str().c_str());

			char buf[1024];
			FILE *fp = _popen(cmd.str().c_str(), "r");

			if (fp == NULL) {
				printf("Error opening pipe!\n");
				return false;
			}

			while (fgets(buf, sizeof(buf), fp) != NULL) {
				printf(" %s", buf);
			}

			if (_pclose(fp)) {
				return false;
			}

			return true;
		}

	private:

#if defined (_WIN32)
		const char *compiler_c_flags = "/Ox";
		const char *compiler_cpp_flags = "/EHsc /Ox";
		const char *linker_flags = "/DLL /SUBSYSTEM:CONSOLE";
#elif (__APPLE__)
		const char *compiler_c_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti -c";
		const char *compiler_cpp_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti -c";
		const char *linker_flags = "-ldl -pthread -shared";
#else
		const char *compiler_c_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti  -c";
		const char *compiler_cpp_flags = "-O3 -std=c++11 -fPIC -Wall -Ofast -fno-rtti  -c";
		const char *linker_flags = "-ldl -pthread -shared";
#endif
		

    };
}

#endif // WEBMVCPP_BUILDER_H
