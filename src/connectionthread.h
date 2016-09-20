#ifndef WEBMVCPP_CONNECTIONTHREAD_H
#define WEBMVCPP_CONNECTIONTHREAD_H

namespace webmvcpp
{
    class core;
	class http_server;

    class http_connection_thread
    {
	    http_connection_thread();
    public:
    explicit http_connection_thread(core_prototype *c, http_server *server, int socket) :
		httpServer(server),
		mvcCore(c),
		socketDescriptor(socket)
	{
	}

	bool
	run()
	{
		bool result = false;
#ifdef _WIN32
		DWORD threadId;
		HANDLE hThread = ::CreateThread(NULL, 0, http_connection_thread::routine, this, 0, &threadId);
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
		result = pthread_create(&threadId, &threadAttr, http_connection_thread::routine, this) == 0;
#endif
		return result;
	}

#ifdef _WIN32
	static DWORD __stdcall
#else
	static void *
#endif
	routine(void *ctx)
	{
		http_connection_thread *_this = static_cast<http_connection_thread *>(ctx);
		std::unique_ptr<http_connection_thread> threadCleaner(_this);

		http_server_connection *connection = new http_server_connection(_this->mvcCore, _this->socketDescriptor);
		std::unique_ptr<http_server_connection>  connectionCleaner(connection);

		connection->run();

		return 0;
	}

    private:
        http_server *httpServer;
        core_prototype *mvcCore;

        int socketDescriptor;
    };
}
#endif // WEBMVCPP_CONNECTIONTHREAD_H
