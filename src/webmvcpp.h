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

#ifdef _WIN32
#include <codecvt>

#include <winsock.h>
#include "dirent_win32.h"
#else
#include <dlfcn.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#endif

#ifdef __linux
#include <sys/auxv.h>
#endif 

#ifdef __APPLE__
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

namespace webmvcpp
{
    class webmvcobject
    {
    public:
		webmvcobject():
        reference(0)
        {}
        virtual ~webmvcobject()
        {}
        unsigned long reference;
    };
}

#define $(c) make_##c

#include "http_parser/http_parser.h"
#include "multipart_parser/multipart_parser.h"
#include "json.hpp"
#include "pugixml/pugixml.hpp"
#include "md5/md5.h"
#include "variant.h"

#include "httprequest.h"
#include "httpresponse.h"
#include "mimefiletypes.h"
#include "requestparser.h"
#include "multipartparser.h"

#include "utils.h"
#include "systemutils.h"
#include "errorpage.h"

#include "application.h"
#include "core.h"