#include "src/webmvcpp.h" 

int main(int argc, char *args[])
{
	if (argc<2)
	{

	}

	bool startAsDaemon = false;
	bool buildOnly = false;
	std::string webApp;

	if (argc == 1) {
		std::cout << "Use parameners:" << std::endl;
		std::cout << "    -d, --daemon\t\t Run as daemon" << std::endl;
		std::cout << "    -b, --build\t\t Build web application, arguments: <.webmvcpp file>" << std::endl;
		std::cout << "    -c, --create\t\t Create new web application, arguments: <name>" << std::endl;
		std::cout << "    -v, --version\t\t Print version" << std::endl;

		return -1;
	}

	for (unsigned int i = 0; i < argc; ++i)
	{
		char *p = args[i];

		if (strcmp(p, "-v") == 0 || strcmp(p, "--version") == 0)
		{
			std::cout << "WebMVC++ Open Source Web Application Engine\nVersion: 0.3." << WEBMVCPP_BUILD_NUMBER << std::endl;
		}
		else if (strcmp(p, "-d") == 0 || strcmp(p, "--daemon") == 0)
		{
			startAsDaemon = true;
			std::cout << "Run webmvc++ as daemon." << std::endl;
		}
		else if ((strcmp(p, "-b") == 0 || strcmp(p, "--build") == 0) && i + 1 < argc)
		{
			buildOnly = true;
		}
		else if ((strcmp(p, "-c") == 0 || strcmp(p, "--create") == 0) && i + 1 < argc)
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
