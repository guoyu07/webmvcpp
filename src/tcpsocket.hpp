#ifndef WEBMVCPP_TCPSOCKET_H
#define WEBMVCPP_TCPSOCKET_H


#define TCPSTREAM_SOCKET_TIMEOUT 20

namespace network {

    struct memory_fragment {
        const unsigned char *buffer;
        const unsigned long size;
    };

    class tcp_socket
    {
        tcp_socket(const tcp_socket &);
        tcp_socket & operator=(const tcp_socket &);

        int socketDescriptor;
    public:
        tcp_socket(int s = socket(AF_INET, SOCK_STREAM, 0), const sockaddr_in addr = {0}):
        socketDescriptor(s)
        {
            remoteAddr = addr;
        }

        ~tcp_socket()
        {
            close();
        }

        bool socket_is_valid() { return socketDescriptor != -1; }
        sockaddr_in & get_socket_addr() {return remoteAddr;}
        
        int detach()
        {
            int socketValue = socketDescriptor;
            socketDescriptor = -1;
            return socketValue;
        }

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
        connect(uint32_t addr, unsigned short port)
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
        listen(uint32_t addr, unsigned short port)
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

        tcp_socket *
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

            
            
            return new tcp_socket(s, cliAddr);
        }

        long
        send(const unsigned char *buffer, unsigned long datalen)
        {
            long sendedBytes = ::send(socketDescriptor, (const char *)buffer, datalen, WEBMVCPP_SENDDATA_FLAGS);
            return sendedBytes;
        }

        long
        recv(unsigned char *buffer, unsigned long datalen)
        {
            long recived = ::recv(socketDescriptor, (char *)buffer, datalen, WEBMVCPP_RECVDATA_FLAGS);
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

        friend tcp_socket& operator<<(tcp_socket& os, const std::string & str)
        {
            os.send((unsigned char *)str.c_str(), str.length());
            return os;
        }
        
        friend tcp_socket& operator<<(tcp_socket& is, const memory_fragment &memptr)
        {
            is.send((unsigned char *)memptr.buffer, memptr.size);
            return is;
        }

        template<typename T> friend tcp_socket& operator<<(tcp_socket& is, const std::vector<T> & data)
        {
            is.send((unsigned char *)&data.front(), data.size()*sizeof(T));
            return is;
        }

        friend tcp_socket& operator>>(tcp_socket& is, std::vector<unsigned char> & data)
        {
            long bytesReceived = is.recv(&data.front(), data.size());
            if (bytesReceived == -1)
                data.resize(0);
            else
                data.resize(bytesReceived);

            return is;
        }

        sockaddr_in remoteAddr;
    };
} // network namespace

#endif // WEBMVCPP_TCPSOCKET_H
