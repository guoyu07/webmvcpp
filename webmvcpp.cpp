#include "src/declarations.hpp"

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
#include "3rdparty/md5.hpp"

#include "src/buildinfo.hpp"

#include "src/utils.hpp"
#include "src/systemutils.hpp"
#include "src/tcpsocket.hpp"

#include "src/httprequest.hpp"
#include "src/httpresponse.hpp"

#include "src/multipartparser.hpp"
#include "src/requestparser.hpp"
#include "src/responseparser.hpp"

#include "src/mvcppshared.hpp"

#include "src/variant.hpp"
#include "src/errorpage.hpp"
#include "src/handlers.hpp"
#include "src/requestmodel.hpp"
#include "src/webapplication.hpp"
#include "src/sessionmanager.hpp"
#include "src/mimefiletypes.hpp"
#include "src/httpclient.hpp"

#include "src/applicationloader.hpp"
#include "src/httpserver.hpp"
#include "src/requestmanager.hpp"
#include "src/builder.hpp"
#include "src/engine.hpp"

#ifdef _WIN32
namespace network {
    void
    initialize()
    {
        WSAData wsaData;
        ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    }
}
#endif


#ifndef WEBMVCPP_AMALGAMATION
const char webmvcpp_headers_hpp_amalgamation[] = {0};
#endif

int main(int argc, char *args[])
{
#if defined (_WIN32)
    network::initialize();
#endif
    bool startAsDaemon = false;
    bool buildOnly = false;
    std::string webApp;

    if (argc == 1) {
        std::cout << "Use parameters:" << std::endl;
        std::cout << "    -D, --daemon\t\t Run as daemon" << std::endl;
        std::cout << "    -B, --build\t\t\t Build web application, arguments: <.webmvcpp file>" << std::endl;
        std::cout << "    -C, --create\t\t Create new web application, arguments: <name>" << std::endl;
        std::cout << "    -T, --test\t\t\t Test application config, arguments: <.webmvcpp file>" << std::endl;
        std::cout << "    -I, --install\t\t Install" << std::endl;
        std::cout << "    -U, --uninstall\t\t Uninstall" << std::endl;
        std::cout << "    -V, --version\t\t Print version" << std::endl;

        return -1;
    }

    webmvcpp::engine mvcEngine;

    for (int i = 0; i < argc; ++i)
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
        else if ((strcmp(p, "-T") == 0 || strcmp(p, "--test") == 0) && i + 1 < argc)
        {
            std::string configFile = args[i + 1];
            mvcEngine.test_configuration(configFile);
            return 0;
        }
        else if ((strcmp(p, "-C") == 0 || strcmp(p, "--create") == 0) && i + 1 < argc)
        {
            std::string appName = args[i + 1];
            mvcEngine.create_application(appName);
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
