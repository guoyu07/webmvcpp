#ifndef WEBMVCPP_MULTIPARTPARSER_H
#define WEBMVCPP_MULTIPARTPARSER_H

namespace webmvcpp
{
    class http_multipart_parser : public webmvcobject
    {
        http_multipart_parser();
        http_multipart_parser(const http_multipart_parser & );
        http_multipart_parser & operator=(const http_multipart_parser & );
    public:
        http_multipart_parser(const std::string & boundary, std::vector<unsigned char> & bodyContent, std::map<std::string, multypart_entry> & results):
		webmvcobject(),
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

        size_t parse();

        static int read_header_name(multipart_parser* p, const char *at, size_t length);
        static int read_header_value(multipart_parser* p, const char *at, size_t length);
        static int read_part_data(multipart_parser* p, const char *at, size_t length);

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