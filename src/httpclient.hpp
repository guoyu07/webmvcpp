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
            network::tcp_socket & s = request.socket;
            request.socket.connect(request.host.c_str(), 80);

            std::ostringstream outData;
            outData << request.method << " " << request.path;
            if (request.queryString.length() > 0) {
                outData << "?" << request.queryString;
            }
            outData << " HTTP/1.1\x0d\x0a";
            outData << "Host: " << request.host << "\x0d\x0a";
            outData << "User-Agent: " << (request.userAgent.length() > 0 ? request.userAgent : "WebMVC++") << "\x0d\x0a";
            outData << "\x0d\x0a";

            s.send((const unsigned char *)outData.str().c_str(), outData.str().length());

            http_response_parser responseParser(response);

            std::vector<unsigned char> recvBuffer;
            recvBuffer.resize(16384);
            do {
                int bytesReceived = s.recv(&recvBuffer.front(), recvBuffer.size());
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
