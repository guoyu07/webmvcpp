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

        std::string status;
        std::string contentType;

        http_values header;

        bool isCompressed;
        bool isKeepAlive;
        std::vector<unsigned char> content;
    };
}
#endif // WEBMVCPP_HTTPRESPONSE_H
