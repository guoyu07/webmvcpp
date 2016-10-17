#ifndef WEBMVCPP_SERVER_CONNECTION_H
#define WEBMVCPP_SERVER_CONNECTION_H

namespace webmvcpp
{
    class http_incoming_connection
    {
        http_incoming_connection();
    public:
        http_incoming_connection(core_prototype *c, http_server_prototype *s, unsigned long ipAddr, network::tcp_socket *socket):
        ipAddress(ipAddr),
        request(*socket),
        response(*socket),
        mvcCore(c),
        httpServer(s),
        httpReqParser(request),
        clientSocket(socket)
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
                    const char *fatalErrorMessage = "HTTP/1.1 500 OK\r\nConnection: close\r\n\r\n<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>";
                    clientSocket->send((const unsigned char *)fatalErrorMessage, strlen(fatalErrorMessage));
                    break;
                }
            } while (request.isKeepAlive);
        }

        unsigned long get_ip_address() { return ipAddress; }
        core_prototype *mvc_core() { return mvcCore; }
        http_server_prototype *http_server() { return httpServer; }
        
        bool
        wait_for_header(http_request & request)
        {
            while (!httpReqParser.is_header_received())
            {
                bool readyRead = request.wait_for_data();
                if (!readyRead)
                    return false;
                
                int rcvBytes = clientSocket->recv(&recvBuffer.front(), recvBuffer.size());
                if (rcvBytes == -1)
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
                
                int rcvBytes = clientSocket->recv(&recvBuffer.front(), recvBuffer.size());
                if (rcvBytes == -1)
                    return false;
                
                httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
            }
            
            return true;
        }
        
        network::tcp_socket *get_client_socket() { return clientSocket; }
    private:
        std::vector<unsigned char> recvBuffer;
        unsigned long ipAddress;

        http_request request;
        http_response response;
        
        core_prototype *mvcCore;
        http_server_prototype *httpServer;
        
        http_request_parser httpReqParser;
        network::tcp_socket * clientSocket;
    };
}

#endif // WEBMVCPP_SERVER_CONNECTION_H
