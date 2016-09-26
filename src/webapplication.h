#ifndef WEBMVCPP_APPLICATION_MODULE_H
#define WEBMVCPP_APPLICATION_MODULE_H


namespace webmvcpp
{
    struct mvc_handlers{
        
        webmvcpp_start_application_fn startApplication;
        webmvcpp_stop_application_fn stopApplication;

        webmvcpp_create_session_fn createSession;
        webmvcpp_remove_session_fn removeSession;

        webmvcpp_check_authorized_fn checkAuthorized;

        webmvcpp_request_handler masterPageHandler;

        std::map<std::string, webmvcpp_view_handler> views;
        std::set<std::string> controllers;
        std::map<std::string, std::map<std::string, request_model>> models;
        std::map<std::string, webmvcpp_request_handler> requests;
        std::map<std::string, webmvcpp_controller_requests_handler> controllerRequests;

        static mvc_handlers *g;
        static mvc_handlers *global() {
            if (g == NULL) {
                g = new mvc_handlers();
            }
            return g;
        };

        ~mvc_handlers() {}
    };

    #define WEBMVC_VIEWDATA "<webmvcpp:content:"
    #define WEBMVC_VIEWDATA_CLOSED "</webmvcpp:content:"
    #define WEBMVC_VIEWCONTROL "<webmvc:control:"
    #define WEBMVC_VIEWCONTROL_CLOSED "</webmvc:control:"
    #define WEBMVC_CLOSEBLOCK_END " />"
    #define WEBMVC_BLOCK_END ">"


    class webapplication
    {

    public:
        webapplication() {}
        virtual ~webapplication() {}

        virtual void start() {}
        virtual void stop() {}

        void acceptCore(core_prototype *c){ mvcCore = c; }

        virtual bool init(const std::string & w, const std::string & s)
        {
            webappPath = w;
            staticPath = s;

            return true;
        }

        mvc_handlers *handlers = mvc_handlers::global();
        
        std::string webappPath;
        std::string staticPath;

        core_prototype *mvcCore;

        std::map<std::string, std::string> routeMap;

        void add_route(const std::string & path, const std::string & newPath)
        {
            routeMap.insert(std::pair<std::string, std::string>(path, newPath));
        }

        bool redirect_to(http_response & response, const std::string & url)
        {
            response.status = "302 found";
            response.header.insert(std::pair<std::string, std::string>("Location", url));

            return true;
        }

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

    class gset_controller_requests_handler
    {
        gset_controller_requests_handler();
    public:
        gset_controller_requests_handler(const std::string & name, webmvcpp_controller_requests_handler fn)
        {
            webmvcpp::mvc_handlers::global()->controllerRequests.insert(std::pair<std::string, webmvcpp_controller_requests_handler>(name, fn));
        }
    };

    class gset_master_page_handler
    {
        gset_master_page_handler();
    public:
        gset_master_page_handler(webmvcpp_request_handler fn)
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
        gadd_view_handler(const std::string & url, webmvcpp_view_handler fn)
        {
            webmvcpp::mvc_handlers::global()->views.insert(std::pair<std::string, webmvcpp_view_handler>(url, fn));
        }
    };

    class gadd_controller
    {
        gadd_controller();
    public:
        gadd_controller(const std::string & name)
        {
            webmvcpp::mvc_handlers::global()->controllers.insert(name);
        }
    };

    class gadd_request_model
    {
        gadd_request_model();
    public:
        gadd_request_model(const std::string & name, const json & reqstsModel)
        {
            std::map<std::string, request_model> reqModelMap;

            for (json::const_iterator it = reqstsModel.begin(); it != reqstsModel.end(); ++it)
            {
                const std::string & methodName = it.key();
                const json & methodObj = it.value();

                for (json::const_iterator methodsIt = methodObj.begin(); methodsIt != methodObj.end(); ++methodsIt)
                {
                    const std::string & reqTypeName = methodsIt.key();

                    if (reqTypeName == "GET")
                    {
                        request_model reqModel;

                        const json & reqModelObj = methodsIt.value();

                        for (json::const_iterator mdlIt = reqModelObj.begin(); mdlIt != reqModelObj.end(); ++mdlIt)
                        {
                            const std::string & mdlProperty = mdlIt.key();

                            if (mdlProperty == MVCPP_MODEL_KEY_FLAGS)
                            {
                                const json & flags = mdlIt.value();
                                for (json::const_iterator flagIt = flags.begin(); flagIt != flags.end(); ++flagIt)
                                {
                                    const json propertyValue = *flagIt;
                                    if (propertyValue.is_string()) {
                                        reqModel.flags.insert(propertyValue.get<std::string>());
                                    }
                                }
                            }
                            else if (mdlProperty == MVCPP_MODEL_KEY_QUERY_STRING)
                            {
                                const json & keys = mdlIt.value();
                                for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
                                {
                                    const json propertyValue = *keysIt;
                                    if (propertyValue.is_string()) {
                                        reqModel.queryString.push_back(propertyValue.get<std::string>());
                                    }
                                }
                            }
                        }

                        reqModelMap.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
                    }
                    else if (reqTypeName == "POST")
                    {
                        request_model reqModel;
                        const json & reqModelObj = methodsIt.value();

                        for (json::const_iterator mdlIt = reqModelObj.begin(); mdlIt != reqModelObj.end(); ++mdlIt)
                        {
                            const std::string & mdlProperty = mdlIt.key();

                            if (mdlProperty == MVCPP_MODEL_KEY_FLAGS)
                            {
                                const json & flags = *mdlIt;
                                for (json::const_iterator flagIt = flags.begin(); flagIt != flags.end(); ++flagIt)
                                {
                                    const json propertyValue = *flagIt;
                                    if (propertyValue.is_string()) {
                                        reqModel.flags.insert(propertyValue.get<std::string>());
                                    }
                                }
                            }
                            else if (mdlProperty == MVCPP_MODEL_KEY_CONTENT_TYPE)
                            {
                                const json propertyValue = mdlIt.value();
                                if (propertyValue.is_string()) {
                                    reqModel.contentType = propertyValue.get<std::string>();
                                }
                            }
                            else if (mdlProperty == MVCPP_MODEL_KEY_QUERY_STRING)
                            {
                                const json keys = mdlIt.value();
                                for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
                                {
                                    const json propertyValue = keysIt.value();
                                    if (propertyValue.is_string()) {
                                        reqModel.queryString.push_back(propertyValue.get<std::string>());
                                    }
                                }
                            }
                            else if (mdlProperty == MVCPP_MODEL_KEY_BODY_ENCODED_PARAMS)
                            {
                                const json keys = mdlIt.value();
                                for (json::const_iterator keysIt = keys.begin(); keysIt != keys.end(); ++keysIt)
                                {
                                    const json propertyValue = keysIt.value();
                                    if (propertyValue.is_string()) {
                                        reqModel.bodyUrlEncodedParams.push_back(propertyValue.get<std::string>());
                                    }
                                }
                            }
                        }

                        reqModelMap.insert(std::pair<std::string, request_model>(reqTypeName, reqModel));
                    }
                    else
                        continue;

                    std::ostringstream requestPath;
                    requestPath << "/" << name << "/" << methodName;

                    webmvcpp::mvc_handlers::global()->models.insert(std::pair<std::string, std::map<std::string, request_model>>(requestPath.str(), reqModelMap));
                }
            }
        }
    };
}

#endif // WEBMVCPP_APPLICATION_MODULE_H