#ifndef WEBMVCPP_SERVER_CONNECTION_H
#define WEBMVCPP_SERVER_CONNECTION_H

namespace webmvcpp
{
    class http_server_connection
    {
        http_server_connection();
    public:
        http_server_connection(core_prototype *c, http_server_prototype *s, unsigned long ipAddr, int socket):
        socketDescriptor(socket),
        ipAddress(ipAddr),
        request(socket),
        response(socket),
        httpReqParser(request),
        mvcCore(c),
        httpServer(s)
        {
            recvBuffer.resize(16 * 1024);
        }

        void
        exec()
        {
            do
            {
                request.clear();
                response.clear();

                try
                {
                    if (!this->wait_for_header(request))
                        break;

                    if (!mvcCore->process_request(this, request, response))
                        request.isKeepAlive = false;
                }
                catch (...)
                {
                    request.isKeepAlive = false;
                    const char *fatalErrorMessage = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>";
                    ::send(socketDescriptor, fatalErrorMessage, strlen(fatalErrorMessage), WEBMVCPP_SENDDATA_FLAGS);
                    break;
                }
            } while (request.isKeepAlive);
        }

        unsigned long get_ip_address() { return ipAddress; }
        core_prototype *mvc_core() { return mvcCore; }
        http_server_prototype *http_server() { return httpServer; }

        void close()
        {
            if (socketDescriptor != -1)
            {
#ifdef _WIN32
                ::closesocket(socketDescriptor);
#else
                ::close(socketDescriptor);
#endif
                socketDescriptor = -1;
            }
            
        }
        
        bool
        wait_for_header(http_request & request)
        {
            while (!httpReqParser.is_header_received())
            {
                bool readyRead = request.wait_for_data();
                if (!readyRead)
                    return false;
                
                size_t rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), WEBMVCPP_RECVDATA_FLAGS);
                if (rcvBytes == 0 || rcvBytes == -1)
                    return false;
                
                httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
            }
            
            return true;
        }
        
        bool
        wait_for_content(http_request & request)
        {
            while (!httpReqParser.is_body_received())
            {
                bool readyRead = request.wait_for_data();
                if (!readyRead)
                    return false;
                
                size_t rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), WEBMVCPP_RECVDATA_FLAGS);
                if (rcvBytes == 0 || rcvBytes == -1)
                    return false;
                
                httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
            }
            
            return true;
        }
        
    private:
        std::vector<unsigned char> recvBuffer;
        unsigned long ipAddress;

        http_request request;
        http_response response;
        
        core_prototype *mvcCore;
        http_server_prototype *httpServer;
        
        http_request_parser httpReqParser;
        int socketDescriptor;
    };
}

#endif // WEBMVCPP_SERVER_CONNECTION_H
