#include "src/webmvcpp.h" 

int main(int argc, char *args[])
{
	bool startAsDaemon = false;
	bool buildOnly = false;
	std::string webApp;

	if (argc == 1) {
		std::cout << "Use parameners:" << std::endl;
		std::cout << "    -D, --daemon\t\t Run as daemon" << std::endl;
		std::cout << "    -B, --build\t\t\t Build web application, arguments: <.webmvcpp file>" << std::endl;
		std::cout << "    -C, --create\t\t Create new web application, arguments: <name>" << std::endl;
        std::cout << "    -I, --daemon\t\t Install" << std::endl;
        std::cout << "    -U, --daemon\t\t Uninstall" << std::endl;
		std::cout << "    -V, --version\t\t Print version" << std::endl;

		return -1;
	}

	for (unsigned int i = 0; i < argc; ++i)
	{
		char *p = args[i];

		if (strcmp(p, "-V") == 0 || strcmp(p, "--version") == 0)
		{
			std::cout << "WebMVC++ Open Source Web Application Engine" << std::endl;
            std::cout << "Version: 0.4." << WEBMVCPP_BUILD_NUMBER << " " << __DATE__ << " at " << __TIME__ << std::endl;
            std::cout << "Compiler: " << WEBMVCPP_COMPILER_CPP << std::endl;
            return 0;
		}
        else if (strcmp(p, "-I") == 0 || strcmp(p, "--install") == 0)
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

                return 0;
        }
        else if (strcmp(p, "-U") == 0 || strcmp(p, "--uninstall") == 0)
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
            std::cout << "Success" << std::endl;
#endif
            return 0;
        }
		else if (strcmp(p, "-D") == 0 || strcmp(p, "--daemon") == 0)
		{
			startAsDaemon = true;
			std::cout << "Run webmvc++ as daemon." << std::endl;
		}
		else if ((strcmp(p, "-B") == 0 || strcmp(p, "--build") == 0) && i + 1 < argc)
		{
			buildOnly = true;
		}
		else if ((strcmp(p, "-C") == 0 || strcmp(p, "--create") == 0) && i + 1 < argc)
		{
			std::string appName = args[i + 1];

		}
		else {
			webApp = p;
		}
	}

	if (webApp.length() == 0)
		return 0;

	webmvcpp::core mvccore;
	if (!mvccore.init(webApp))
	{
		return -2;
	}

	mvccore.build_applications();
	
	if (buildOnly)
		return 0;

	mvccore.load_applications();

	if (mvccore.start(startAsDaemon, argc, args))
	{
		return -3;
	}

	return 0;
}
