#ifndef WEBMVCPP_APPLICATION_MODULE_H
#define WEBMVCPP_APPLICATION_MODULE_H

#include "handlers.h"
#include "requestmodel.h"

namespace webmvcpp
{
    struct mvc_handlers{
        
        webmvcpp_start_application_fn startApplication;
        webmvcpp_stop_application_fn stopApplication;

        webmvcpp_create_session_fn createSession;
        webmvcpp_remove_session_fn removeSession;

        webmvcpp_check_authorized_fn checkAuthorized;

        webmvcpp_view_handler masterPageHandler;

        std::map<std::string, webmvcpp_view_handler> views;
        std::map<std::string, webmvcpp_request_handler> requests;

        static mvc_handlers *g;
        static mvc_handlers *global() {
            if (g == NULL) {
                g = new mvc_handlers();
            }
            return g;
        };
    };

    class application
    {

    public:
        application();
        virtual ~application();

        void start();
        void stop();

        void acceptCore(core *c){ mvcCore = c; }

		virtual bool init(const std::string & w, const std::string & s);

        mvc_handlers *handlers = mvc_handlers::global();

        std::map<std::string, std::string> pages;
        std::set<std::string> controllers;
        std::map<std::string, std::map<std::string, request_model>> reqModels;
        
        std::string webappPath;
        std::string staticPath;

        core *mvcCore;

        std::map<std::string, std::string> routeMap;

        void add_route(const std::string & path, const std::string & newPath);

        bool redirect_to(http_response & response, const std::string & url);

        virtual bool get_masterpage(std::string & pageFile);

		bool init_models();

        bool init_controllers();

		bool init_pages();
	private:

    };
    
    class gset_start_application_handler
    {
        gset_start_application_handler();
    public:
        gset_start_application_handler(webmvcpp_start_application_fn fn)
        {
            webmvcpp::mvc_handlers::global()->startApplication = fn;
        }
    };

    class gset_stop_application_handler
    {
        gset_stop_application_handler();
    public:
        gset_stop_application_handler(webmvcpp_stop_application_fn fn)
        {
            webmvcpp::mvc_handlers::global()->stopApplication = fn;
        }
    };

    class gset_create_session_handler
    {
        gset_create_session_handler();
    public:
        gset_create_session_handler(webmvcpp_create_session_fn fn)
        {
            webmvcpp::mvc_handlers::global()->createSession = fn;
        }

    };

    class gset_remove_session_handler
    {
        gset_remove_session_handler();
    public:
        gset_remove_session_handler(webmvcpp_remove_session_fn fn)
        {
            webmvcpp::mvc_handlers::global()->removeSession = fn;
        }
    };

    class gadd_request_handler
    {
        gadd_request_handler();
    public:
        gadd_request_handler(const std::string & url, webmvcpp_request_handler fn)
        {
            webmvcpp::mvc_handlers::global()->requests.insert(std::pair<std::string, webmvcpp_request_handler>(url, fn));
        }
    };

    class gset_master_page_handler
    {
        gset_master_page_handler();
    public:
        gset_master_page_handler(webmvcpp_view_handler fn)
        {
            webmvcpp::mvc_handlers::global()->masterPageHandler = fn;
        }
    };

    class gset_check_authorized_handler
    {
        gset_check_authorized_handler();
    public:
        gset_check_authorized_handler(webmvcpp_check_authorized_fn fn)
        {
            webmvcpp::mvc_handlers::global()->checkAuthorized = fn;
        }
    };

    class gadd_view_handler
    {
        gadd_view_handler();
    public:
        gadd_view_handler(const std::string & url, webmvcpp_view_handler fn)\
        {
            webmvcpp::mvc_handlers::global()->views.insert(std::pair<std::string, webmvcpp_view_handler>(url, fn));
        }
    };
}

#endif // WEBMVCPP_APPLICATION_MODULE_H