#include "src/webmvcpp.h" 

int main(int argc, char *args[])
{
	if (argc<2)
	{
		std::cout << "Use parameners:" << std::endl << "    -d, --daemon\t\t Run as daemon" << std::endl << "    -v, --version\t\t Print version" << std::endl;

		return -1;
	}

	bool startAsDaemon = false;
	std::string webApp;
	for (int i = 1; i < argc; ++i)
	{
		const char *argument = args[i];
		if (strcmp(argument, "-v") == 0 || strcmp(argument, "--version") == 0)
		{
			std::cout << "WebMVC++ Open Source Web Application Engine\nVersion: 0.1." << WEBMVCPP_BUILD_NUMBER << std::endl;
		}
		else if (strcmp(argument, "-d") == 0 || strcmp(argument, "--daemon") == 0)
		{
			startAsDaemon = true;
			std::cout << "Run webmvc++ as daemon." << std::endl;
		}
		else {
			webApp = argument;
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
	
	if (mvccore.start(startAsDaemon, argc, args))
	{
		return -3;
	}

	return 0;
}
