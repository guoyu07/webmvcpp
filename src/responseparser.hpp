#ifndef WEBMVCPP_RESPONSE_PARSER_H
#define WEBMVCPP_RESPONSE_PARSER_H

namespace webmvcpp
{
    class connection;

    class http_response_parser
    {
        http_response_parser(void);
    public:
        static int header_field_cb(http_parser *p, const char *buf, size_t len)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            _this->currentHeaderField = std::string(buf, len);

            return 0;
        }

        static int header_value_cb(http_parser *p, const char *buf, size_t len)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            _this->response.header.insert(std::pair<std::string, std::string>(_this->currentHeaderField, std::string(buf, len)));

            return 0;
        }
        
        static int response_status_cb(http_parser *p, const char *buf, size_t len)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            std::ostringstream responseStatus;
            responseStatus << p->status_code << " " << std::string(buf, len);
            _this->response.status = responseStatus.str();

            return 0;
        }

        static int body_cb(http_parser *p, const char *buf, size_t len)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            utils::append_bytes(_this->response.content, (const unsigned char *)buf, len);

            return 0;
        }

        static int message_begin_cb(http_parser *p)
        {
            (void)p;

            return 0;
        }

        static int headers_complete_cb(http_parser *p)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            _this->headerReceived = true;

            return 0;
        }

        static int message_complete_cb(http_parser *p)
        {
            http_response_parser *_this = static_cast<http_response_parser *>(p->data);
            _this->bodyReceived = true;

            return 0;
        }

        http_response_parser(http_response & httpRes) :
        response(httpRes),
        headerReceived(false),
        bodyReceived(false)
        {
            httpParser.data = this;
            http_parser_init(&httpParser, HTTP_RESPONSE);

            httpParserSettings.on_message_begin = http_response_parser::message_begin_cb;
            httpParserSettings.on_header_field = http_response_parser::header_field_cb;
            httpParserSettings.on_header_value = http_response_parser::header_value_cb;
            httpParserSettings.on_url = NULL;// http_response_parser::request_url_cb;
            httpParserSettings.on_status = http_response_parser::response_status_cb;
            httpParserSettings.on_body = http_response_parser::body_cb;
            httpParserSettings.on_headers_complete = http_response_parser::headers_complete_cb;
            httpParserSettings.on_message_complete = http_response_parser::message_complete_cb;
        }

        size_t accept_data(const unsigned char * buffer, size_t length)
        {
            return http_parser_execute(&httpParser, &httpParserSettings, (const char *)buffer, length);
        }

        bool is_header_received() { return headerReceived; }
        bool is_body_received() { return bodyReceived; }
    private:
        http_response & response;

        http_parser_settings httpParserSettings;
        http_parser httpParser;

        bool headerReceived;
        bool bodyReceived;

        std::string currentHeaderField;
    };
}

#endif // WEBMVCPP_RESPONSE_PARSER_H