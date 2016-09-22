#include "src/declarations.h"

#include "3rdparty/intrusive_ptr.hpp"

#if defined(_WIN32)
#include "3rdparty/dirent_win32.h"
#endif
extern "C" {
#include "3rdparty/http_parser/http_parser.h"
#include "3rdparty/multipart_parser/multipart_parser.h"
}
#include "3rdparty/json.hpp"
#include "3rdparty/pugixml/pugixml.hpp"
#include "3rdparty/pugixml/pugixml.cpp"
#include "3rdparty/md5.h"

#include "src/buildinfo.h"

#include "src/utils.h"
#include "src/systemutils.h"

#include "src/httprequest.h"
#include "src/httpresponse.h"

#include "src/mvcppshared.h"

#include "src/connection.h"

#include "src/variant.h"
#include "src/errorpage.h"
#include "src/multipartparser.h"
#include "src/handlers.h"
#include "src/requestmodel.h"
#include "src/webapplication.h"
#include "src/requestparser.h"
#include "src/sessionmanager.h"
#include "src/mimefiletypes.h"
#include "src/serverconnection.h"
#include "src/requestmanager.h"


#include "src/applicationloader.h"
#include "src/connectionthread.h"
#include "src/server.h"
#include "src/builder.h"
#include "src/engine.h"


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

        webmvcpp::engine mvcEngine;

	for (unsigned int i = 0; i < argc; ++i)
	{
		char *p = args[i];

		if (strcmp(p, "-V") == 0 || strcmp(p, "--version") == 0)
		{
                     mvcEngine.display_version();
                     return 0;
		}
        else if (strcmp(p, "-I") == 0 || strcmp(p, "--install") == 0)
        {
            mvcEngine.install();
            return 0;
        }
        else if (strcmp(p, "-U") == 0 || strcmp(p, "--uninstall") == 0)
        {
            mvcEngine.uninstall();
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

	if (!mvcEngine.init(webApp))
	{
		return -2;
	}

	mvcEngine.build_applications();
	
	if (buildOnly)
		return 0;

	mvcEngine.load_applications();

	if (mvcEngine.start(startAsDaemon, argc, args))
	{
		return -3;
	}

	return 0;
}
