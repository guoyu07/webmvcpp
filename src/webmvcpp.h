#pragma once 

#include "declarations.h"

#include "boost/intrusive_ptr.hpp"

#if defined(_WIN32)
#include "dirent_win32.h"
#endif
#include "http_parser/http_parser.h"
#include "multipart_parser/multipart_parser.h"
#include "json.hpp"
#include "pugixml/pugixml.hpp"
#include "md5/md5.h"

#include "buildinfo.h"

#include "utils.h"
#include "systemutils.h"

#include "httprequest.h"
#include "httpresponse.h"

#include "mvcppshared.h"

#include "connection.h"

#include "variant.h"
#include "errorpage.h"
#include "multipartparser.h"
#include "handlers.h"
#include "requestmodel.h"
#include "webapplication.h"
#include "requestparser.h"
#include "sessionmanager.h"
#include "mimefiletypes.h"
#include "serverconnection.h"
#include "requestmanager.h"


#include "applicationloader.h"
#include "connectionthread.h"
#include "server.h"
#include "builder.h"
#include "webmvcppcore.h"
