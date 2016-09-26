#ifndef WEBMVCPP_MULTIPARTPARSER_H
#define WEBMVCPP_MULTIPARTPARSER_H

namespace webmvcpp
{
    class http_multipart_parser
    {
        http_multipart_parser();
        http_multipart_parser(const http_multipart_parser & );
        http_multipart_parser & operator=(const http_multipart_parser & );
    public:
        http_multipart_parser(const std::string & boundary, std::vector<unsigned char> & bodyContent, std::map<std::string, multypart_entry> & results):
        bodyContent(bodyContent),
        results(results),
        parserCtx(NULL)
        {
            memset(&parserSettings, 0, sizeof(parserSettings));

            parserSettings.on_header_field = http_multipart_parser::read_header_name;
            parserSettings.on_header_value = http_multipart_parser::read_header_value;
            parserSettings.on_part_data = http_multipart_parser::read_part_data;

            parserCtx = multipart_parser_init(boundary.c_str(), &parserSettings);
            multipart_parser_set_data(parserCtx, this);
        }

        ~http_multipart_parser()
        {
            multipart_parser_free(parserCtx);
        }

        size_t parse()
        {
            unsigned int retSize = multipart_parser_execute(parserCtx, (const char *)&bodyContent.front(), bodyContent.size());

            return retSize;
        }

        static int read_header_name(multipart_parser *p, const char *at, size_t length)
        {
            http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

            _this->currentKeyName = std::string(at, length);

            return 0;
        }

        static int read_header_value(multipart_parser *p, const char *at, size_t length)
        {
            http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

            std::string value(at, length);

            _this->currentMultipartContent.headers.insert(std::pair<std::string, std::string>(_this->currentKeyName, value));

            if (_this->currentKeyName == "Content-Disposition")
            {
                std::map<std::string, std::string> contentDispParams;

                const std::vector<std::string> params = utils::split_string(value, ';');
                for (std::vector<std::string>::const_iterator it = params.cbegin(); it != params.cend(); ++it)
                {
                    const std::string & oneParam = *it;

                    size_t oneParamValueSep = oneParam.find('=');
                    if (oneParamValueSep != std::string::npos)
                    {
                        std::string contentParamKey = oneParam.substr(0, oneParamValueSep);
                        contentParamKey = utils::trim_string(contentParamKey);

                        std::string contentParamValue = oneParam.substr(oneParamValueSep + 1);

                        if (contentParamValue.length() > 2 && (contentParamValue[0] == '\'' || contentParamValue[0] == '\"') && contentParamValue[0] == contentParamValue[contentParamValue.length() - 1])
                        {
                            contentParamValue = contentParamValue.substr(1, contentParamValue.length() - 2);
                        }

                        contentDispParams.insert(std::pair<std::string, std::string>(contentParamKey, contentParamValue));
                    }
                }

                std::map<std::string, std::string>::const_iterator it = contentDispParams.find("name");
                if (it != contentDispParams.cend())
                    _this->currentContentName = it->second;
            }

            return 0;
        }

        static int read_part_data(multipart_parser* p, const char *at, size_t length)
        {
            http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

            utils::append_bytes(_this->currentMultipartContent.content, (unsigned char *)at, length);

            return 0;
        }

    private:
        std::vector<unsigned char> & bodyContent;

        std::map<std::string, multypart_entry> & results;

        std::string currentContentName;
        multypart_entry currentMultipartContent;
        std::string currentKeyName;

        multipart_parser *parserCtx;
        multipart_parser_settings parserSettings;
    };
}

#endif // WEBMVCPP_MULTIPARTPARSER_H
