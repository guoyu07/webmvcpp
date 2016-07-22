#ifndef WEBMVCPP_HTTPSERVER_H
#define WEBMVCPP_HTTPSERVER_H

namespace webmvcpp
{
    class core;

    class http_server_impl : public webmvcobject
    {
        http_server_impl();
    public:
        explicit http_server_impl(core *c);
        bool start_listening(unsigned short port);

        void stop();

    private:
        void close();

        core *mvcCore;

        int listenSocket;
        bool running;
    };

    typedef boost::intrusive_ptr<http_server_impl> http_server;
}

#endif // WEBMVCPP_HTTPSERVER_H
