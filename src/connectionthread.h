#ifndef WEBMVCPP_CONNECTIONTHREAD_H
#define WEBMVCPP_CONNECTIONTHREAD_H

#include "server.h"

namespace webmvcpp
{
    class core;

    class http_connection_thread
    {
	    http_connection_thread();
    public:
        explicit http_connection_thread(core *app, http_server server, int socket);

        bool run();

#ifdef _WIN32
    static DWORD __stdcall routine(void *ctx);
#else
    static void *routine(void *ctx);
#endif

    private:
        http_server httpServer;
        core *mvcCore;

        int socketDescriptor;
    };
}
#endif // WEBMVCPP_CONNECTIONTHREAD_H
