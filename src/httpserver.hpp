#ifndef WEBMVCPP_HTTPSERVER_H
#define WEBMVCPP_HTTPSERVER_H

namespace webmvcpp
{
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
                    http_incoming_connection *connection = new http_incoming_connection(mvcCore, this, ipAddr, clientSocket);
                    retain_connection(ipAddr, connection);
                    if (!systemutils::create_thread(connection_thread_routine, connection))
                    {
                        release_connection(ipAddr, connection);
                        delete connection;
                        delete clientSocket;
                    }
                }
            } while (running);

            return true;
        }

#ifdef _WIN32
        static DWORD __stdcall
#else
        static void *
#endif
        connection_thread_routine(void *threadParam)
        {
            http_incoming_connection *connection = static_cast<http_incoming_connection *>(threadParam);
            http_server_prototype *server = connection->http_server();
            unsigned long ipAddress = connection->get_ip_address();
            std::unique_ptr<http_incoming_connection> threadCleaner(connection);
            std::unique_ptr<network::tcp_socket> socketCleaner(connection->get_client_socket());

            if (server->is_connection_permitted(ipAddress))
            {
                connection->exec();
            }

            server->release_connection(ipAddress, connection);

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
        retain_connection(unsigned long ipAddress, http_incoming_connection *c)
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);

            activeConnections.insert(c);

            ipConnections[ipAddress]++;
        }

        void
        release_connection(unsigned long ipAddress, http_incoming_connection *c)
        {
            std::unique_lock<std::mutex> lm(connectionsCountLock);

            activeConnections.erase(c);

            ipConnections[ipAddress]--;
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
        std::set<http_incoming_connection *> activeConnections;
        std::map<unsigned long, unsigned long> ipConnections;

        bool running;
    };
}

#endif // WEBMVCPP_HTTPSERVER_H
