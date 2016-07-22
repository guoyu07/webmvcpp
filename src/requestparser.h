#ifndef WEBMVCPP_REQUEST_PARSER_H
#define WEBMVCPP_REQUEST_PARSER_H

namespace webmvcpp
{
    class connection;

    class http_request_parser : public webmvcobject
    {
        http_request_parser(void);
    public:
        http_request_parser(http_request & req);

        static int header_field_cb (http_parser *p, const char *buf, size_t len);
        static int header_value_cb (http_parser *p, const char *buf, size_t len);
        static int request_url_cb (http_parser *p, const char *buf, size_t len);
        static int response_status_cb (http_parser *p, const char *buf, size_t len);
        static int body_cb (http_parser *p, const char *buf, size_t len);
        static int message_begin_cb (http_parser *p);
        static int headers_complete_cb (http_parser *p);
        static int message_complete_cb (http_parser *p);

        int accept_data(const unsigned char *rcvBytes, unsigned int length);

        bool is_header_received(){return headerReceived;}
        bool is_body_received(){return bodyReceived;}

    private:
        http_request & request;

        http_parser_settings httpParserSettings;
        http_parser httpParser;

        bool headerReceived;
        bool bodyReceived;

        std::string currentHeaderField;
    };
}

#endif // WEBMVCPP_REQUEST_PARSER_H