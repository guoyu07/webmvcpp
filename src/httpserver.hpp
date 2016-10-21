#ifndef WEBMVCPP_HTTPSERVER_H
#define WEBMVCPP_HTTPSERVER_H

namespace webmvcpp
{
    struct http_connection_context
    {
    public:
        http_connection_context(core_prototype *core, http_server_prototype *server, network::tcp_socket *socket, const unsigned long ip):
        ipAddress(ip),
        clientSocket(socket->detach(), socket->get_socket_addr()),
        request(clientSocket),
        response(clientSocket),
        mvcCore(core),
        httpServer(server),
        httpReqParser(request)
        {}

        unsigned long ipAddress;

        network::tcp_socket clientSocket;

        http_request request;
        http_response response;

        core_prototype *mvcCore;
        http_server_prototype *httpServer;
        
        http_request_parser httpReqParser;
    };

    class http_server : public http_server_prototype
    {
        http_server();
    public:
        explicit http_server(core_prototype *c) :
        mvcCore(c),
        maxConnections(100),
        connectionsPerIp(10),
        maximumTimeout(30),
        running(false)
        {}

        bool
        start_listening(unsigned short port, unsigned long maxCon, unsigned long conPerIP, unsigned long maxTimeout)
        {
            running = true;
            maxConnections = maxCon;
            connectionsPerIp = conPerIP;
            maximumTimeout = maxTimeout;

            do
            {
                network::tcp_socket serverSocket;
                if (!serverSocket.socket_is_valid())
                {
                    webmvcpp::systemutils::sleep(10 * 1000);
                    continue;
                }

                if (!serverSocket.listen(port))
                {
                    webmvcpp::systemutils::sleep(10 * 1000);
                    continue;
                }

                while (running)
                {
                    if (!is_new_connection_permited())
                        continue;

                    network::tcp_socket *clientSocket = serverSocket.accept();
                    if (clientSocket == NULL)
                    {
                        break;
                    }

                    unsigned long ipAddr = clientSocket->remoteAddr.sin_addr.s_addr;
                    http_connection_context *ctx = new http_connection_context(mvcCore, this, clientSocket, ipAddr);
                    
                    retain_connection(ctx);
                    if (!systemutils::create_thread(connection_thread_routine, ctx))
                    {
                        release_connection(ctx);
                        delete clientSocket;
                    }
                }
            } while (running);

            return true;
        }
        
        
        static bool wait_for_header(http_connection_context *ctx)
        {
            http_request & request = ctx->request;
            http_request_parser & httpReqParser = ctx->httpReqParser;
            std::vector<unsigned char> recvBuffer;
            recvBuffer.resize(2048);
            while (!httpReqParser.is_header_received())
            {
                bool readyRead = request.wait_for_data();
                if (!readyRead)
                    return false;
                
                ctx->clientSocket >> recvBuffer;
                if (recvBuffer.size()==0) {
                    return false;
                }
                
                httpReqParser.accept_data(&recvBuffer.front(), recvBuffer.size());
            }
            
            return true;
        }

#ifdef _WIN32
        static DWORD __stdcall
#else
        static void *
#endif
        connection_thread_routine(void *threadParam)
        {
            http_connection_context *ctx = static_cast<http_connection_context *>(threadParam);
            http_server_prototype *server = ctx->httpServer;
            std::unique_ptr<http_connection_context> ctxCleaner(ctx);

            if (server->is_connection_permitted(ctx->ipAddress))
            {
                http_request & request = ctx->request;
                http_response & response = ctx->response;
                do
                {
                    request.clear();
                    response.clear();
                    
                    try
                    {
                        if (!wait_for_header(ctx))
                            break;
                        
                        if (!ctx->mvcCore->process_request(ctx))
                            request.isKeepAlive = false;
                    }
                    catch (...)
                    {
                        request.isKeepAlive = false;
                        error_page::send(ctx->response, 500, "Internal Server Error", "<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>");
                        break;
                    }
                } while (request.isKeepAlive);
            }

            server->release_connection(ctx);

            return 0;
        }

        bool
        is_new_connection_permited()
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);
            return connection_conditions.wait_for(lm, std::chrono::seconds(maximumTimeout), [this]() { return this->activeConnections.size() < this->maxConnections; });
        }

        bool
        is_connection_permitted(unsigned long ipAddress)
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);
            return connection_conditions.wait_for(lm, std::chrono::seconds(maximumTimeout), [this, ipAddress]() {
                std::map<unsigned long, unsigned long>::const_iterator it = this->ipConnections.find(ipAddress);
                return it == this->ipConnections.end() || it->second < this->connectionsPerIp;
            });
        }

        void 
        retain_connection(http_connection_context *ctx)
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);

            activeConnections.insert(ctx);

            ipConnections[ctx->ipAddress]++;
        }

        void
        release_connection(http_connection_context *ctx)
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);

            activeConnections.erase(ctx);

            ipConnections[ctx->ipAddress]--;
        }

        void
        stop()
        {
            running = false;
        }

        core_prototype *mvcCore;

        unsigned long maxConnections;
        unsigned long connectionsPerIp;
        unsigned long maximumTimeout;

        std::condition_variable connection_conditions;
        std::mutex connectionsCountLock;
        std::set<http_connection_context *> activeConnections;
        std::map<unsigned long, unsigned long> ipConnections;

        bool running;
    };
}

#endif // WEBMVCPP_HTTPSERVER_H
