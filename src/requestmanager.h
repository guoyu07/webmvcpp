#ifndef REQUESTMANAGER_CORE_H
#define REQUESTMANAGER_CORE_H

#include "sessionmanager.h"
#include "requestmodel.h"

namespace webmvcpp
{
    class core;
    class application;

    class request_manager
	{
	public:
        request_manager(core *c);

        void process_request(application *mvcapp, http_connection connection, http_request & request, http_response & response);

    private:
        void send_mvc_page(application *mvcapp, http_connection connection, http_request & request, http_response & response);
        void send_static_file(application *webapp, http_connection connection, http_request & request, http_response & response);

        bool is_model_valid(application *mvcapp, http_connection connection, http_request & request, session sessionContext, const std::map<std::string, request_model> &m);

        core  *webMvcCore;
	};
}

#endif // REQUESTMANAGER_CORE_H
