#ifndef WEBMVCPP_HTTPRESPONSE_H
#define WEBMVCPP_HTTPRESPONSE_H

namespace webmvcpp
{
    struct http_response
    {
        http_response()
        {
            clear();
        }

        void clear()
        {
            status.clear();
            contentType.clear();

            header.clear();

            isCompressed = false;
            isKeepAlive = false;

            content.clear();
        }

        void add_cookie(const std::pair<std::string, std::string> & keyValue) 
        {
            header.insert(keyValue);
        }

        void add_cookie(const std::string & key, const std::string & value)
        {
            add_cookie(std::pair<std::string, std::string>(key, value));
        }

        std::string status;
        std::string contentType;

        std::multimap<std::string, std::string> header;

        bool isCompressed;
        bool isKeepAlive;
        std::vector<unsigned char> content;
    };
}
#endif // WEBMVCPP_HTTPRESPONSE_H
