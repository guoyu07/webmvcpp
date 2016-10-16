#ifndef WEBMVCPP_HTTPCLIENT_H
#define WEBMVCPP_HTTPCLIENT_H

namespace webmvcpp
{
    class http_client
    {
    public:
        http_client()
        {
        }
        
        bool do_sync(const http_request & request, http_response & response)
        {
            int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == -1)
            {
                return false;
            }

            const hostent *hostinfo = gethostbyname(request.host.c_str());
            
            sockaddr_in serv_addr = {0};
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr = *(struct in_addr *) hostinfo->h_addr;
            serv_addr.sin_port = htons(80);

            if (::connect(clientSocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                return false;

            std::ostringstream outData;
            outData << request.method << " " << request.path;
            if (request.queryString.length() > 0) {
                outData << "?" << request.queryString;
            }
            outData << " HTTP/1.1\x0d\x0a";
            outData << "Host: " << request.host << "\x0d\x0a";
            outData << "User-Agent: " << (request.userAgent.length() > 0 ? request.userAgent : "WebMVC++") << "\x0d\x0a";
            outData << "\x0d\x0a";

            std::string outDataStr = outData.str();
            ::send(clientSocket, outDataStr.c_str(), outDataStr.length(), WEBMVCPP_SENDDATA_FLAGS);

            http_response_parser responseParser(response);

            std::vector<unsigned char> recvBuffer;
            recvBuffer.resize(16384);
            bool recvResult = false;
            do {
                int bytesReceived = ::recv(clientSocket, (char *)&recvBuffer.front(), recvBuffer.size(), WEBMVCPP_RECVDATA_FLAGS);
                if (bytesReceived < 0)
                    return false;
                responseParser.accept_data(&recvBuffer.front(), bytesReceived);
            } while (!responseParser.is_body_received() || !responseParser.is_header_received());

            return true;
        }
        

    private:

    };
}

#endif // WEBMVCPP_HTTPCLIENT_H
