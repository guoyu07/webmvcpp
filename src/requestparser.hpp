#ifndef WEBMVCPP_REQUEST_PARSER_H
#define WEBMVCPP_REQUEST_PARSER_H

namespace webmvcpp
{
    class connection;

    class http_request_parser
    {
        http_request_parser(void);
    public:
        static int header_field_cb(http_parser *p, const char *buf, size_t len)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);
            _this->currentHeaderField = std::string(buf, len);

            return 0;
        }

        static int header_value_cb(http_parser *p, const char *buf, size_t len)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);
            _this->request.header.insert(std::pair<std::string, std::string>(_this->currentHeaderField, std::string(buf, len)));

            return 0;
        }

        static int request_url_cb(http_parser *p, const char *buf, size_t len)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);

            http_request & request = _this->request;
            request.url = std::string(buf, len);

            size_t urlParamsPos = request.url.find('?');
            if (urlParamsPos != std::string::npos)
            {
                request.path = request.url.substr(0, urlParamsPos);
                request.queryString = request.url.substr(urlParamsPos + 1);

                std::vector<std::string> paramsList = utils::split_string(request.queryString, '&');
                for (std::vector<std::string>::const_iterator it = paramsList.cbegin(); it != paramsList.cend(); ++it)
                {
                    const std::string & oneParam = *it;

                    size_t splitedPos = oneParam.find('=');
                    if (splitedPos != std::string::npos)
                    {
                        request.getParams.insert(std::pair<std::string, std::string>(oneParam.substr(0, splitedPos), oneParam.substr(splitedPos + 1)));
                    }
                }
            }
            else
                request.path = request.url;

            return 0;
        }

        /*
        static int HttpRequestParser::response_status_cb (http_parser *p, const char *buf, size_t len)
        {
        return 0;
        }
        */

        static int body_cb(http_parser *p, const char *buf, size_t len)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);
            utils::append_bytes(_this->request.content, (const unsigned char *)buf, len);

            return 0;
        }

        static int message_begin_cb(http_parser *p)
        {
            (void)p;

            return 0;
        }

        static int headers_complete_cb(http_parser *p)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);

            http_request & request = _this->request;

            _this->headerReceived = true;

            switch (p->method)
            {
            case HTTP_GET:
                request.method = "GET";
                break;
            case HTTP_POST:
                request.method = "POST";
                break;
            }

            std::map<std::string, std::string>::iterator realmIt = request.header.find("Authorization");
            if (realmIt != request.header.end())
            {
                std::string authorizationValue = realmIt->second;
                size_t authParamSepPos = authorizationValue.find(' ');
                if (authParamSepPos != std::string::npos)
                {
                    std::string auth1param = authorizationValue.substr(0, authParamSepPos);
                    std::transform(auth1param.begin(), auth1param.end(), auth1param.begin(), ::tolower);
                    if (auth1param == "basic")
                    {
                        std::string auth2param = authorizationValue.substr(authParamSepPos + 1);
                        std::string encodedRealm = utils::base64_decode(auth2param);
                        size_t realmSep = encodedRealm.find(':');
                        if (realmSep != std::string::npos)
                        {
                            request.authRealm.first = encodedRealm.substr(0, realmSep);
                            request.authRealm.second = encodedRealm.substr(realmSep + 1);
                        }
                    }
                }
            }

            /*
            std::map<std::string, std::string>::iterator it = request.header.find("Accept-Encoding");
            if (it!=request.header.end() && it->find("deflate")!=std::string::npos)
            request.supportGzip = true;
            */

            std::map<std::string, std::string>::iterator connectionIt = request.header.find("Connection");
            if (connectionIt != request.header.end() && connectionIt->second.find("Keep-Alive") != std::string::npos)
                request.isKeepAlive = true;

            std::map<std::string, std::string>::iterator refererIt = request.header.find("Referer");
            if (refererIt != request.header.end())
                request.referer = refererIt->second;

            std::map<std::string, std::string>::iterator userAgentIt = request.header.find("User-Agent");
            if (userAgentIt != request.header.end())
                request.userAgent = userAgentIt->second;

            std::map<std::string, std::string>::iterator hostIt = request.header.find("Host");
            if (hostIt != request.header.end()) {
                request.host = utils::split_string(hostIt->second, ':')[0];
            }


            std::map<std::string, std::string>::iterator contentTypeIt = request.header.find("Content-Type");
            if (contentTypeIt != request.header.end())
                request.contentType = contentTypeIt->second;

            std::map<std::string, std::string>::iterator rangesIt = request.header.find("Range");
            if (rangesIt != request.header.end())
            {
                std::string rangeValue = rangesIt->second;

                std::vector<std::string> ranges = utils::split_string(rangeValue, ',');
                for (std::vector<std::string>::iterator rangeIt = ranges.begin(); rangeIt != ranges.end(); ++rangeIt)
                {
                    std::string oneRange = *rangeIt;

                    std::vector<std::string> rangeValues = utils::split_string(oneRange, '-');

                    if (rangeValues.size() == 2)
                    {
                        std::pair<uint64_t, uint64_t> range;

                        if (rangeValues[0].length())
                        {
                            std::istringstream rangeFromStr(rangeValues[0]);
                            rangeFromStr >> range.first;
                        }
                        else
                            range.first = -1;

                        if (rangeValues[1].length())
                        {
                            std::istringstream rangeToStr(rangeValues[1]);
                            rangeToStr >> range.second;
                        }
                        else
                            range.first = -1;

                        request.ranges.push_back(range);
                    }
                }

                request.rangesExist = true;
            }

            const std::vector<std::string> splittedGetParams = utils::split_string(request.queryString, '&');
            for (std::vector<std::string>::const_iterator it = splittedGetParams.begin(); it != splittedGetParams.end(); ++it)
            {
                const std::string & param = *it;

                size_t paramSep = param.find('=');
                if (paramSep != std::string::npos)
                    request.getParams.insert(std::pair<std::string, std::string>(param.substr(0, paramSep), param.substr(paramSep + 1)));
            }

            return 0;
        }

        static int message_complete_cb(http_parser *p)
        {
            http_request_parser *_this = static_cast<http_request_parser *>(p->data);
            _this->bodyReceived = true;

            if (p->method != HTTP_POST)
                return 0;

            http_request & request = _this->request;
            if (request.contentType.find("application/x-www-form-urlencoded") != std::string::npos)
            {
                std::string paramsStr(request.content.begin(), request.content.end());

                const std::vector<std::string> splittedPostParams = utils::split_string(paramsStr, '&');
                for (std::vector<std::string>::const_iterator it = splittedPostParams.begin(); it != splittedPostParams.end(); ++it)
                {
                    const std::string & param = *it;

                    size_t paramSep = param.find('=');
                    if (paramSep != std::string::npos)
                        request.postParams.insert(std::pair<std::string, std::string>(param.substr(0, paramSep), param.substr(paramSep + 1)));
                }
            }
            else if (request.contentType.find("multipart/form-data") != std::string::npos)
            {
                std::string boundaryKey = "boundary=";
                size_t boundaryPos = request.contentType.find(boundaryKey);
                if (boundaryPos != std::string::npos)
                {
                    boundaryPos += boundaryKey.length();
                    size_t boundaryEnd = request.contentType.find(';', boundaryPos);

                    std::string boundaryValue = std::string("--") + request.contentType.substr(boundaryPos, boundaryEnd - boundaryPos);

                    http_multipart_parser multipartParser(boundaryValue, request.content, request.multipartData);
                    multipartParser.parse();
                }
            }

            return 0;
        }

        http_request_parser(http_request & httpReq) :
        request(httpReq),
        headerReceived(false),
        bodyReceived(false)
        {
            httpParser.data = this;
            http_parser_init(&httpParser, HTTP_REQUEST);

            httpParserSettings.on_message_begin = http_request_parser::message_begin_cb;
            httpParserSettings.on_header_field = http_request_parser::header_field_cb;
            httpParserSettings.on_header_value = http_request_parser::header_value_cb;
            httpParserSettings.on_url = http_request_parser::request_url_cb;
            httpParserSettings.on_status = NULL;//http_request_parser::response_status_cb;
            httpParserSettings.on_body = http_request_parser::body_cb;
            httpParserSettings.on_headers_complete = http_request_parser::headers_complete_cb;
            httpParserSettings.on_message_complete = http_request_parser::message_complete_cb;
        }

        size_t accept_data(const unsigned char * buffer, size_t length)
        {
            return http_parser_execute(&httpParser, &httpParserSettings, (const char *)buffer, length);
        }

        bool is_header_received() { return headerReceived; }
        bool is_body_received() { return bodyReceived; }
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