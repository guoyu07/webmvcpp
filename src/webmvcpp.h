#pragma once 

#include <algorithm>
#include <functional> 

#include <memory>
#include <string>
#include <list>
#include <deque>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <mutex>
#include <thread>
#include <locale>
#include <atomic>

#include <cctype>
#include <locale>
#include <iomanip>

#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <codecvt>
#include <winsock.h>
#include "dirent_win32.h"
#else
#include <dlfcn.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#endif

#ifdef __linux
#include <unistd.h>
#include <pwd.h>
#include <sys/auxv.h>
#endif 

#ifdef __APPLE__
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h> 
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include "boost/intrusive_ptr.hpp"

#include "http_parser/http_parser.h"
#include "multipart_parser/multipart_parser.h"
#include "json.hpp"
#include "pugixml/pugixml.hpp"
#include "md5/md5.h"

using namespace nlohmann;

#include "buildinfo.h"

#include "utils.h"
#include "systemutils.h"

#include "mimefiletypes.h"

#include "httprequest.h"
#include "httpresponse.h"

namespace webmvcpp
{
	class core;
	class http_server;
	class http_connection;
	class session_manager;

	class core_prototype {
	public:
		virtual bool process_request(http_connection *connection, http_request & request, http_response & response) = 0;
		virtual session_manager *get_session_manager() = 0;
		virtual mime_file_types *get_mime_types() = 0;
	};
}

#define $(c) make_##c

#include "variant.h"
#include "errorpage.h"
#include "applicationbuilder.h"
#include "multipartparser.h"
#include "handlers.h"
#include "requestmodel.h"
#include "webapplication.h"
#include "requestparser.h"
#include "sessionmanager.h"
#include "connection.h"
#include "requestmanager.h"


#include "applicationloader.h"
#include "connectionthread.h"
#include "server.h"
#include "builder.h"
#include "webmvcppcore.h"
