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
		listenSocket(-1),
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
				listenSocket = socket(AF_INET, SOCK_STREAM, 0);
				if (listenSocket == -1)
				{
					webmvcpp::systemutils::sleep(10 * 1000);
					continue;
				}

				sockaddr_in servAddr;
				sockaddr_in cliAddr;

				memset((char *)&servAddr, 0, sizeof(servAddr));

				servAddr.sin_family = AF_INET;
				servAddr.sin_port = htons(port);
				servAddr.sin_addr.s_addr = INADDR_ANY;
#ifdef _WIN32
				const char yes = 1;
#else
				const int yes = 1;
#endif
				if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
				{
					close();

					webmvcpp::systemutils::sleep(10 * 1000);
					continue;
				}

				if (bind(listenSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1)
				{
					close();

					webmvcpp::systemutils::sleep(10 * 1000);
					continue;
				}

				if (listen(listenSocket, SOMAXCONN) != 0)
				{
					close();

					webmvcpp::systemutils::sleep(10 * 1000);
					continue;
				}

				
				while (running)
				{
					if (!is_new_connection_permited())
						continue;
#ifdef _WIN32
					int cliAddrLen = 0;
#else
					socklen_t cliAddrLen = 0;
#endif

					cliAddrLen = sizeof(cliAddr);

					int clientSocket = accept(listenSocket, (struct sockaddr *) &cliAddr, &cliAddrLen);
					if (clientSocket == -1)
					{
						close();
						break;
					}

					unsigned long ipAddr = cliAddr.sin_addr.s_addr;
					http_server_connection *connection = new http_server_connection(mvcCore, this, ipAddr, clientSocket);
					retain_connection(ipAddr, connection);
					if (!run_connection_thread_routine(connection))
					{
						connection->close();
						release_connection(ipAddr, connection);
						delete connection;
					}
				}
			} while (running);

			return true;
		}

		bool
		run_connection_thread_routine(http_server_connection *c)
		{
			bool result = false;
#ifdef _WIN32
			DWORD threadId;
			HANDLE hThread = ::CreateThread(NULL, 0, connection_thread_routine, c, 0, &threadId);
			if (hThread != NULL)
			{
				::CloseHandle(hThread);
				result = true;
			}
#else
			pthread_t threadId;
			pthread_attr_t threadAttr;
			pthread_attr_init(&threadAttr);
			pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
			result = pthread_create(&threadId, &threadAttr, connection_thread_routine, c) == 0;
#endif
			return result;
		}

#ifdef _WIN32
		static DWORD __stdcall
#else
		static void *
#endif
		connection_thread_routine(void *threadParam)
		{
			http_server_connection *connection = static_cast<http_server_connection *>(threadParam);
			std::unique_ptr<http_server_connection> threadCleaner(connection);

			http_server_prototype *server = connection->http_server();

			unsigned long ipAddress = connection->get_ip_address();

			if (server->is_connection_permitted(ipAddress))
				connection->run();
			else
				connection->close();

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
		retain_connection(unsigned long ipAddress, http_server_connection *c)
		{
			std::unique_lock<std::mutex> lm(connectionsCountLock);

			activeConnections.insert(c);

			ipConnections[ipAddress]++;
		}

		void
		release_connection(unsigned long ipAddress, http_server_connection *c)
		{
			std::unique_lock<std::mutex> lm(connectionsCountLock);

			activeConnections.erase(c);

			ipConnections[ipAddress]--;
		}

		void
		stop()
		{
			running = false;

			close();
		}




    private:
		void
		close()
		{
			if (listenSocket != -1)
			{
#ifdef _WIN32
				::closesocket(listenSocket);
#else
				::close(listenSocket);
#endif
				listenSocket = -1;
			}
		}

		core_prototype *mvcCore;

		unsigned long maxConnections;
		unsigned long connectionsPerIp;
		unsigned long maximumTimeout;

		std::condition_variable connection_conditions;
		std::mutex connectionsCountLock;
		std::set<http_server_connection *> activeConnections;
		std::map<unsigned long, unsigned long> ipConnections;

        int listenSocket;
        bool running;
    };
}

#endif // WEBMVCPP_HTTPSERVER_H
