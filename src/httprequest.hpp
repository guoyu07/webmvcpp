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
    private:
        http_request();
        http_request(const http_request &);
        http_request & operator=(const http_request &);
    public:
        http_request(network::tcp_socket & s, const std::string & m, const std::string & h, const std::string & p, const http_values & q = http_values()) :
        method(m),
        path(p),
        host(h),
        socket(s)
        {
            for (http_values::const_iterator it = q.begin(); it != q.end(); ++it)
            {
                queryString += it->first + "=" + it->second;
                if (std::next(it) != q.end()) queryString += "&";
            }

            url = q.size() == 0 ? path : path + "?" + queryString;
        }

        http_request(network::tcp_socket & s):
        socket(s)
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
        
        bool
        read_stream(std::vector<unsigned char> & streamData)
        {
            bool readyRead = this->wait_for_data();
            if (!readyRead)
                return readyRead;
            
            streamData.resize(64 * 1024);
            
            socket >> streamData;
            
            return streamData.size() != 0;
        }
        
        bool
        wait_for_data()
        {
            timeval t = { 30, 0 };
            return socket.wait_for_read(&t);
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
        
        network::tcp_socket & socket;
    };
}
#endif // WEBMVCPP_HTTPREQUEST_H
