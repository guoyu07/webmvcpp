#ifndef WEBMVCPP_HTTPSERVER_H
#define WEBMVCPP_HTTPSERVER_H

namespace webmvcpp
{
    class http_server
    {
        http_server();
    public:
		explicit http_server(core_prototype *c) :
		mvcCore(c),
		listenSocket(0),
		running(false)
		{}

		bool
		start_listening(unsigned short port)
		{
			running = true;

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

				do
				{
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

					http_connection_thread *thread = new http_connection_thread(mvcCore, this, clientSocket);
					if (!thread->run())
						delete thread;
				} while (running);
			} while (running);

			return true;
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
			}
		}

		core_prototype *mvcCore;

        int listenSocket;
        bool running;
    };
}

#endif // WEBMVCPP_HTTPSERVER_H
