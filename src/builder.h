#ifndef WEBMVCPP_BUILDER_H
#define WEBMVCPP_BUILDER_H

#ifdef DEBUG
const char webmvcpp_headers_hpp_amalgamation[] = { 0 };
const char webmvcpp_sources_cpp_amalgamation[] = { 0 };
const char webmvcpp_http_parser_c[] = { 0 };
const char webmvcpp_http_parser_h[] = { 0 };
const char webmvcpp_multipart_parser_c[] = { 0 };
const char webmvcpp_multipart_parser_h[] = { 0 };
#endif

extern "C" const char webmvcpp_headers_hpp_amalgamation[];
extern "C" const char webmvcpp_sources_cpp_amalgamation[];
extern "C" const char webmvcpp_http_parser_c[];
extern "C" const char webmvcpp_http_parser_h[];
extern "C" const char webmvcpp_multipart_parser_c[];
extern "C" const char webmvcpp_multipart_parser_h[];

namespace webmvcpp
{
    class builder
    {
		builder();
    public:
        builder(const std::string & appName, const std::string & webAppPath):
		applicationName(appName),
		webApplicationPath(webAppPath),
		headersHppAmalgamation(webAppPath + "/webmvcpp.hpp"),
		sourcesCppAmalgamation(webAppPath + "/webmvcpp.cpp"),
		headerHttpParser(webAppPath + "/webmvcpp_http_parser.h"),
		sourcesHttpParser(webAppPath + "/webmvcpp_http_parser.c"),
		headerMultipartParser(webAppPath + "/webmvcpp_multipart_parser.h"),
		sourcesMultipartParser(webAppPath + "/webmvcpp_multipart_parser.c")
		{
			std::ofstream outputHeadersHppFile(headersHppAmalgamation, std::ofstream::out);
			outputHeadersHppFile << webmvcpp_headers_hpp_amalgamation;
			outputHeadersHppFile.close();

			std::ofstream outputSourcesCppFile(sourcesCppAmalgamation, std::ofstream::out);
			outputSourcesCppFile << webmvcpp_sources_cpp_amalgamation;
			outputSourcesCppFile.close();

			std::ofstream httpParserHFile(headerHttpParser, std::ofstream::out);
			httpParserHFile << webmvcpp_http_parser_h;
			httpParserHFile.close();

			std::ofstream httpParserCFile(sourcesHttpParser, std::ofstream::out);
			httpParserCFile << webmvcpp_http_parser_c;
			httpParserCFile.close();

			std::ofstream multipartParserHFile(headerMultipartParser, std::ofstream::out);
			multipartParserHFile << webmvcpp_multipart_parser_h;
			multipartParserHFile.close();

			std::ofstream multipartParserCFile(sourcesMultipartParser, std::ofstream::out);
			multipartParserCFile << webmvcpp_multipart_parser_c;
			multipartParserCFile.close();
		}

		virtual ~builder()
		{
#if defined (_WIN32)
			::DeleteFileA(headersHppAmalgamation.c_str());
			::DeleteFileA(sourcesCppAmalgamation.c_str());
			::DeleteFileA(headerHttpParser.c_str());
			::DeleteFileA(sourcesHttpParser.c_str());
			::DeleteFileA(headerMultipartParser.c_str());
			::DeleteFileA(sourcesMultipartParser.c_str());
#else
			remove(headersHppAmalgamation.c_str());
			remove(sourcesCppAmalgamation.c_str());
			remove(headerHttpParser.c_str());
			remove(sourcesHttpParser.c_str());
			remove(headerMultipartParser.c_str());
			remove(sourcesMultipartParser.c_str());
#endif
		}

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

			ofs << "LIBRARY \"" << applicationName << ".dll\"" << std::endl;
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
		std::string sourcesCppAmalgamation;
		std::string headerHttpParser;
		std::string sourcesHttpParser;
		std::string headerMultipartParser;
		std::string sourcesMultipartParser;

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

#endif // WEBMVCPP_BUILDER_H
