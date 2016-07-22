#ifndef WEBMVCPP_HTTPREQUEST_H
#define WEBMVCPP_HTTPREQUEST_H

namespace webmvcpp
{
    typedef std::map<std::string, std::string> http_values;

    struct multypart_entry
    {
        http_values headers;
	    std::vector<unsigned char> content;

        void clear()
        {
            content.clear();
            headers.clear();
        }
    };

    typedef std::map<std::string, multypart_entry> http_multypart_values;

    struct http_request
    {
        http_request()
        {
		    clear();
	    }

        void clear()
        {
            method.clear();
            url.clear();
            host.clear();
            path.clear();
            queryString.clear();
            contentType.clear();
            referer.clear();
            userAgent.clear();

            rangesExist = false;
            chunckedContent = false;
            isKeepAlive = false;
            supportGZip = false;
            contentLength = 0;
            ranges.clear();

            authRealm.first.clear();
            authRealm.second.clear();

            getParams.clear();
            postParams.clear();
            multipartData.clear();

		    header.clear();

            content.clear();
        }

        bool rangesExist;
        bool supportGZip;
        bool isKeepAlive;
        bool chunckedContent;
        uint64_t contentLength;
        std::vector<std::pair<int64_t, int64_t> > ranges;

        std::string method;
        std::string url;

        std::string path;
        std::string queryString;

        std::string host;

        std::string contentType;
        std::string referer;
        std::string userAgent;

	    std::pair<std::string, std::string> authRealm;

	    http_values getParams;
        http_values postParams;
        http_multypart_values multipartData;

        http_values header;

        std::vector<unsigned char> content;
    };
}
#endif // WEBMVCPP_HTTPREQUEST_H
