#include "webmvcpp.h" 
#include "connectionthread.h"
#include "connection.h"

namespace webmvcpp
{
    http_connection_thread::http_connection_thread(core *c, http_server server, int socket):
    httpServer(server),
    mvcCore(c),
    socketDescriptor(socket)
    {
    }

    bool 
    http_connection_thread::run()
    {
        bool result = false;
#ifdef _WIN32
        DWORD threadId;
        HANDLE hThread = ::CreateThread(NULL, 0, http_connection_thread::routine, this, 0, &threadId);
        if (hThread!=NULL)
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
    DWORD
#else
    void *
#endif
    http_connection_thread::routine(void *ctx)
    {
        http_connection_thread *_this = static_cast<http_connection_thread *>(ctx);
        std::unique_ptr<http_connection_thread> threadCleaner(_this);

        http_connection connection = new http_connection_impl(_this->mvcCore, _this->socketDescriptor);

        connection->run();

        return 0;
    }
}