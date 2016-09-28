#ifndef WEBMVCPP_SERVER_CONNECTION_H
#define WEBMVCPP_SERVER_CONNECTION_H

namespace webmvcpp
{
    class http_server_connection : public http_connection
    {
        http_server_connection();
    public:
        http_server_connection(core_prototype *c, http_server_prototype *s, unsigned long ipAddr, int socket):
        http_connection(socket),
        ipAddress(ipAddr),
        httpReqParser(request),
        mvcCore(c),
        httpServer(s)
        {

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

                    if (!wait_for_header())
                        break;

                    if (!mvcCore->process_request(this, request, response))
                        request.isKeepAlive = false;
                }
                catch (...)
                {
                    request.isKeepAlive = false;
                    const char *fatalErrorMessage = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>";
                    ::send(socketDescriptor, fatalErrorMessage, strlen(fatalErrorMessage), sendDataFlags);
                    break;
                }
            } while (request.isKeepAlive);
        }

        bool
        wait_for_header()
        {
            while (!httpReqParser.is_header_received())
            {
                bool readyRead = this->wait_for_data();
                if (!readyRead)
                    return false;

                int rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), recvDataFlags);
                if (rcvBytes == 0 || rcvBytes == -1)
                    return false;

                httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
            }

            return true;
        }

        bool
        wait_for_content()
        {
            while (!httpReqParser.is_body_received())
            {
                bool readyRead = this->wait_for_data();
                if (!readyRead)
                    return false;

                int rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), recvDataFlags);
                if (rcvBytes == 0 || rcvBytes == -1)
                    return false;

                httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
            }

            return true;
        }

        unsigned long get_ip_address() { return ipAddress; }
        core_prototype *mvc_core() { return mvcCore; }
        http_server_prototype *http_server() { return httpServer; }

    private:
        unsigned long ipAddress;
        http_request request;
        http_response response;

        http_request_parser httpReqParser;

        core_prototype *mvcCore;
        http_server_prototype *httpServer;
    };
}

#endif // WEBMVCPP_SERVER_CONNECTION_H
