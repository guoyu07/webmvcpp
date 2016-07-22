#include "webmvcpp.h" 

namespace webmvcpp
{
    http_connection_impl::http_connection_impl(core *c, int socket) :
    webmvcobject(),
    httpReqParser(request),
    socketDescriptor(socket),
    mvcCore(c)
    {
        recvBuffer.resize(16*1024);
    }

    void
        http_connection_impl::run()
    {
        do
        {
            request.clear();
            response.clear();
           
            try
            {

                if (!wait_for_header())
                    break;

                if (!mvcCore->process_request(this, request, response))
                    request.isKeepAlive = false;
            }
            catch(...)
            {
                request.isKeepAlive = false;
                const char *fatalErrorMessage = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>";
                send(socketDescriptor, fatalErrorMessage, strlen(fatalErrorMessage), 0);
                break;
            }
        }
        while(request.isKeepAlive);        

#ifdef _WIN32
        ::closesocket(socketDescriptor);
#else
        ::close(socketDescriptor);
#endif
    }

    void
    http_connection_impl::send_response_header(const http_response & response)
    {
        std::ostringstream os;

        os << "HTTP/1.1 " << response.status << "\r\n";
        os << "Content-type: " << response.contentType << "\r\n";

        for (std::map<std::string, std::string>::const_iterator it = response.header.cbegin(); it != response.header.cend(); ++it)
            os << it->first << ": " << it->second << "\r\n";

        os << "Transfer-Encoding: chunked\r\n";
        //if (response.contentCompressed)
        //    os << "Content-Encoding: deflate\r\n";
        if (response.isKeepAlive)
            os << "Connection: Keep-Alive\r\n";
        else
            os << "Connection: Close\r\n";
        os << "\r\n";

        ::send(socketDescriptor, os.str().c_str(), os.str().length(), 0);
    }

    void
    http_connection_impl::send_response_content(std::ifstream & fs, const std::vector<std::pair<int64_t, int64_t> > & ranges)
    {
        std::vector<unsigned char> fileBuffer;
        fileBuffer.resize(64*1024);

        std::vector<std::pair<int64_t, int64_t> >::const_iterator it;

        for(it = ranges.begin();it < ranges.end();++it)
        {
            std::pair<int64_t, int64_t> range = *it;

            int64_t length = range.second - range.first;

            fs.seekg(range.first, fs.cur);

            std::streamoff rPos = 0;
            while(rPos!=length)
            {
                std::streamoff rSize = min(64*1024, length - rPos);
                fs.read((char *)&fileBuffer.front(), rSize);
                
                std::ostringstream hexStrm;
                hexStrm << std::hex << rSize << "\r\n";
                std::string hexStr = hexStrm.str();
                
                ::send(socketDescriptor, hexStr.c_str(), hexStr.length(), 0);
                ::send(socketDescriptor, (const char *)&fileBuffer.front(), (int)rSize, 0);
                ::send(socketDescriptor, "\r\n", 2, 0);

                rPos += rSize;
            }
        }
    }

    void
    http_connection_impl::send_response_content(std::ifstream & fs)
    {
        std::vector<unsigned char> fileBuffer;
        fileBuffer.resize(64*1024);

        fs.seekg (0, fs.end);
        std::streamoff length = fs.tellg();
        fs.seekg (0, fs.beg);

        std::streamoff rPos = 0;
        while(rPos!=length)
        {
            std::streamoff rSize = min(64*1024, length - rPos);
            fs.read((char *)&fileBuffer.front(), rSize);
            
            std::ostringstream hexStrm;
            hexStrm << std::hex << rSize << "\r\n";
            std::string hexStr = hexStrm.str();
            
            send(socketDescriptor, hexStr.c_str(), hexStr.length(), 0);
            send(socketDescriptor, (const char *)&fileBuffer.front(), (int)rSize, 0);
            send(socketDescriptor, "\r\n", 2, 0);

            rPos += rSize;
        }
    }

    void
    http_connection_impl::send_response_content(const std::vector<unsigned char> & bytes)
    {
        int cPtr = 0;
        int bufferLength = bytes.size();

        while(bufferLength != cPtr)
        {
            int cFragmentLength = min(64*1024, bufferLength - cPtr);

            std::ostringstream hexStrm;
            hexStrm << std::hex << cFragmentLength << "\r\n";
            std::string hexStr = hexStrm.str();

            send(socketDescriptor, hexStr.c_str(), hexStr.length(), 0);
            send(socketDescriptor, (const char *)&bytes.front() + cPtr, cFragmentLength, 0);
            send(socketDescriptor, "\r\n", 2, 0);
            
            cPtr += cFragmentLength;
        }
    }

    void
    http_connection_impl::send_response_content(const std::string & text)
    {
        int cPtr = 0;
        int bufferLength = text.length();

        while(bufferLength != cPtr)
        {
            int cFragmentLength = min(64*1024, bufferLength - cPtr);

            std::ostringstream hexStrm;
            hexStrm << std::hex << cFragmentLength << "\r\n";
            std::string hexStr = hexStrm.str();
            
            send(socketDescriptor, hexStr.c_str(), hexStr.length(), 0);
            send(socketDescriptor, (const char *)text.c_str() + cPtr, cFragmentLength, 0);
            send(socketDescriptor, "\r\n", 2, 0);

            cPtr += cFragmentLength;
        }
    }

    void
    http_connection_impl::end_response()
    {
        ::send(socketDescriptor, "0\r\n\r\n", 5, 0);
    }

    bool
    http_connection_impl::wait_for_data()
    {
	    fd_set readfds;
	    FD_ZERO(&readfds);
        FD_SET(socketDescriptor, &readfds);
	
        timeval t = {30, 0};
	    int ret = select(socketDescriptor + 1, &readfds, NULL, NULL, &t);
	    if (ret==0 || ret==-1)
		    return false;

	    if( FD_ISSET(socketDescriptor, &readfds))
		    return true;

	    return false;
    }

    bool
    http_connection_impl::wait_for_header()
    {       
        while(!httpReqParser.is_header_received())
        {
            bool readyRead = this->wait_for_data();
            if (!readyRead)
                return false;

            int rcvBytes = recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), 0);
            if (rcvBytes==0 || rcvBytes==-1)
                return false;
            
            httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
        }       
        
	    return true;
    }
    
    bool
    http_connection_impl::wait_for_content()
    {
        while(!httpReqParser.is_body_received())
        {
            bool readyRead = this->wait_for_data();
            if (!readyRead)
                return false;

            int rcvBytes = recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), 0);
            if (rcvBytes==0 || rcvBytes==-1)
                return false;

            httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
        }

	    return true;
    }

    bool
    http_connection_impl::read_stream(std::vector<unsigned char> & streamData)
    {
        bool readyRead = this->wait_for_data();
        if (!readyRead)
            return readyRead;

        streamData.resize(64*1024);

        recv(socketDescriptor, (char *)&streamData.front(), streamData.size(), 0);

        return readyRead;
    }
}