#ifndef WEBMVCPP_TCPSOCKET_H
#define WEBMVCPP_TCPSOCKET_H


#define TCPSTREAM_SOCKET_TIMEOUT 20

namespace network {

    class tcp_socket
    {
        tcp_socket(const tcp_socket &);
        tcp_socket & operator=(const tcp_socket &);

        int socketDescriptor;
    public:
        tcp_socket():
        socketDescriptor(socket(AF_INET, SOCK_STREAM, 0))
        {
            memset(&remoteAddr, 0, sizeof(remoteAddr));
        }

        tcp_socket(int s) :
        socketDescriptor(s)
        {
            memset(&remoteAddr, 0, sizeof(remoteAddr));
        }

        ~tcp_socket()
        {
            close();
        }

        bool socket_is_valid() { return socketDescriptor != -1; }

        void close()
        {
            if (socketDescriptor != -1)
            {
#ifdef _WIN32
                ::closesocket(socketDescriptor);
#else
                ::close(socketDescriptor);
#endif
                socketDescriptor = -1;
            }
        }

        bool
        connect(unsigned long addr, unsigned short port)
        {
            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_port = htons(port);
            remoteAddr.sin_addr.s_addr = addr;

            if (::connect(socketDescriptor, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) == -1)
                return false;

            return true;
        }

        bool
        connect(const char *target, unsigned short port)
        {
            hostent* hostinfo = gethostbyname(target);
            if (!hostinfo)
                return false;

            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_port = htons(port);
            remoteAddr.sin_addr = *(struct in_addr *)hostinfo->h_addr;

            if (::connect(socketDescriptor, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) == -1)
                return false;

            return true;
        }

        bool
        listen(unsigned long addr, unsigned short port)
        {
            remoteAddr.sin_addr.s_addr = addr;
            remoteAddr.sin_family = AF_INET;
            remoteAddr.sin_port = htons(port);

            const int yes = 1;
            if (::setsockopt(socketDescriptor, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(int)) == -1)
                return false;

            if (::bind(socketDescriptor, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr)) == -1)
                return false;

            if (::listen(socketDescriptor, SOMAXCONN) != 0)
                return false;

            return true;
        }

        bool
        listen(unsigned short port)
        {
            return listen(INADDR_ANY, port);
        }

        tcp_socket*
        accept()
        {
            sockaddr_in cliAddr;
#ifdef _WIN32
            int cliAddrLen = 0;
#else
            socklen_t cliAddrLen = 0;
#endif
            cliAddrLen = sizeof(cliAddr);

            int s = ::accept(socketDescriptor, (struct sockaddr *)&cliAddr, &cliAddrLen);
            if (s == -1)
                return NULL;

            tcp_socket* acceptedStream = new tcp_socket(s);
            acceptedStream->remoteAddr = cliAddr;
            return acceptedStream;
        }

        int
        send(const unsigned char *buffer, unsigned long datalen)
        {
            unsigned long sendedBytes = ::send(socketDescriptor, (const char *)buffer, datalen, WEBMVCPP_SENDDATA_FLAGS);
            return sendedBytes;
        }

        int
        recv(unsigned char *buffer, unsigned long datalen)
        {
            unsigned long recived = ::recv(socketDescriptor, (char *)buffer, datalen, WEBMVCPP_RECVDATA_FLAGS);
            if (recived == -1 || recived == 0)
                return -1;

            return recived;
        }

        bool wait_for_read(timeval *tValue)
        {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(socketDescriptor, &readfds);

            int ret = ::select(socketDescriptor + 1, &readfds, NULL, NULL, tValue);
            if (ret == 0 || ret == -1)
                return false;

            if (FD_ISSET(socketDescriptor, &readfds))
                return true;

            return false;
        }

        sockaddr_in remoteAddr;
    };
} // network namespace

#endif // WEBMVCPP_TCPSOCKET_H
