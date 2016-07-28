#ifndef WEBMVCPP_MVCHANDLERS_H
#define WEBMVCPP_MVCHANDLERS_H

namespace webmvcpp
{
    typedef std::map<std::string, std::string> mvc_view_data;
     
    typedef bool (*webmvcpp_start_application_fn)();
    typedef void (*webmvcpp_stop_application_fn)();

    typedef bool (*webmvcpp_create_session_fn)(const http_request & request, variant_map & sessionData);
    typedef void (*webmvcpp_remove_session_fn)(variant_map & sessionData);

    typedef bool (*webmvcpp_check_authorized_fn)(http_request & request, variant_map & session);

    typedef bool (*webmvcpp_request_handler)(http_connection *connection, http_request & request, http_response & response, variant_map & session);
    typedef bool (*webmvcpp_view_handler)(http_connection *connection, http_request & request, http_response & response, variant_map & session, mvc_view_data & viewData);

    #define set_start_application_handler(fn) webmvcpp::gset_start_application_handler g_set_start_application_handler(fn);

    #define set_stop_application_handler(fn) webmvcpp::gset_stop_application_handler g_set_stop_application_handler(fn);

    #define set_create_session_handler(fn) webmvcpp::gset_create_session_handler g_set_create_session_handler(fn);

    #define set_remove_session_handler(fn) webmvcpp::gset_remove_session_handler g_set_remove_session_handler(fn);

    #define set_masterpage_handler(fn) webmvcpp::gset_master_page_handler g_set_master_page_handler(fn);

    #define request_handler(controler, method, fn) webmvcpp::gadd_request_handler g_add_request_handler_##controler##method ("/"#controler"/"#method, fn);

    #define view_handler(controler, method, fn) webmvcpp::gadd_view_handler g_add_view_handler_##controler##method ("/"#controler"/"#method, fn);

    #define check_authorization_handler(fn) webmvcpp::gset_check_authorized_handler g_set_check_authorized_handle(fn);
}

#endif // WEBMVCPP_MVCHANDLERS_H
