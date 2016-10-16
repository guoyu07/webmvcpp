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
        http_request(const std::string & m, const std::string & h, const std::string & u, const http_values & q = http_values()) :
        socketDescriptor(0),
        method(m),
        host(h),
        url(u)
        {
            for (http_values::const_iterator it = q.begin(); it != q.end(); ++it)
            {
                queryString += it->first + "=" + it->second;
                if (std::next(it) != q.end()) queryString += "&";
            }
        }

        http_request(int s):
        socketDescriptor(s)
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
            
            ::recv(socketDescriptor, (char *)&streamData.front(), streamData.size(), WEBMVCPP_RECVDATA_FLAGS);
            
            return readyRead;
        }
        
        bool
        wait_for_data()
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(socketDescriptor, &readfds);
            
            timeval t = { 30, 0 };
            int ret = select(socketDescriptor + 1, &readfds, NULL, NULL, &t);
            if (ret == 0 || ret == -1)
                return false;
            
            if (FD_ISSET(socketDescriptor, &readfds))
                return true;
            
            return false;
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
        
        int socketDescriptor;
    };
}
#endif // WEBMVCPP_HTTPREQUEST_H
