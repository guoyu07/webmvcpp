#include "webmvcpp.h" 

namespace webmvcpp
{
    size_t
    http_multipart_parser::parse()
    {
        unsigned int retSize = multipart_parser_execute(parserCtx, (const char *)&bodyContent.front(), bodyContent.size());

        return retSize;
    }

    int http_multipart_parser::read_header_name(multipart_parser *p, const char *at, size_t length)
    {
        http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

        _this->currentKeyName = std::string(at, length);

       return 0;
    }

    int http_multipart_parser::read_header_value(multipart_parser *p, const char *at, size_t length)
    {
        http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

        std::string value(at, length);

        _this->currentMultipartContent.headers.insert(std::pair<std::string, std::string>(_this->currentKeyName, value));

        if (_this->currentKeyName=="Content-Disposition")
        {
            std::map<std::string, std::string> contentDispParams;

            const std::vector<std::string> params = split_string(value, ';');
            for(std::vector<std::string>::const_iterator it = params.cbegin();it!=params.cend();++it)
            {
                const std::string & oneParam = *it;

                size_t oneParamValueSep = oneParam.find('=');
                if (oneParamValueSep != std::string::npos)
                {
                    std::string contentParamKey = oneParam.substr(0, oneParamValueSep);
                    contentParamKey = trim_string(contentParamKey);

                    std::string contentParamValue = oneParam.substr(oneParamValueSep + 1);

                    if (contentParamValue.length() > 2 && (contentParamValue[0]=='\'' || contentParamValue[0]=='\"') && contentParamValue[0] == contentParamValue[contentParamValue.length() - 1])
                    {
                        contentParamValue = contentParamValue.substr(1, contentParamValue.length() - 2);
                    }

                    contentDispParams.insert(std::pair<std::string, std::string>(contentParamKey, contentParamValue));
                }
            }

            std::map<std::string, std::string>::const_iterator it = contentDispParams.find("name");
            if (it!=contentDispParams.cend())
                _this->currentContentName = it->second;
        }

        return 0;
    }

    int http_multipart_parser::read_part_data(multipart_parser* p, const char *at, size_t length)
    {
        http_multipart_parser *_this = static_cast<http_multipart_parser *>(multipart_parser_get_data(p));

        append_bytes(_this->currentMultipartContent.content, (unsigned char *)at, length);

        return 0;
    }
}