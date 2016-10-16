#ifndef WEBMVCPP_HTTPRESPONSE_H
#define WEBMVCPP_HTTPRESPONSE_H

namespace webmvcpp
{
    struct http_response
    {
    private:
        http_response();
        http_response(const http_response &);
        http_response & operator=(const http_response &);
    public:
        http_response(int s):
        socketDescriptor(s)
        {
            clear();
        }

        void clear()
        {
            status.clear();
            contentType.clear();

            header.clear();

            isCompressed = false;
            isKeepAlive = false;
        }
        
        void end()
        {
            ::send(socketDescriptor, "0\r\n\r\n", 5, WEBMVCPP_SENDDATA_FLAGS);
        }
        
        void
        send_header() const
        {
            std::ostringstream os;
            
            os << "HTTP/1.1 " << this->status << "\r\n";
            os << "Content-type: " << this->contentType << "\r\n";
            
            for (std::map<std::string, std::string>::const_iterator it = this->header.cbegin(); it != this->header.cend(); ++it)
                os << it->first << ": " << it->second << "\r\n";
            
            os << "Transfer-Encoding: chunked\r\n";
            //if (response.contentCompressed)
            //    os << "Content-Encoding: deflate\r\n";
            if (this->isKeepAlive)
                os << "Connection: Keep-Alive\r\n";
            else
                os << "Connection: Close\r\n";
            os << "\r\n";
            
            ::send(socketDescriptor, os.str().c_str(), os.str().length(), WEBMVCPP_SENDDATA_FLAGS);
        }
        
        void
        send_content(std::ifstream & fs, const std::vector<std::pair<int64_t, int64_t> > & ranges)
        {
            std::vector<unsigned char> fileBuffer;
            fileBuffer.resize(64 * 1024);
            
            std::vector<std::pair<int64_t, int64_t> >::const_iterator it;
            
            for (it = ranges.begin(); it < ranges.end(); ++it)
            {
                std::pair<int64_t, int64_t> range = *it;
                
                int64_t length = range.second - range.first;
                
                fs.seekg(range.first, fs.cur);
                
                std::streamoff rPos = 0;
                while (rPos != length)
                {
                    std::streamoff rSize = min(64 * 1024, length - rPos);
                    fs.read((char *)&fileBuffer.front(), rSize);
                    
                    std::ostringstream hexStrm;
                    hexStrm << std::hex << rSize << "\r\n";
                    std::string hexStr = hexStrm.str();
                    
                    ::send(socketDescriptor, hexStr.c_str(), hexStr.length(), WEBMVCPP_SENDDATA_FLAGS);
                    ::send(socketDescriptor, (const char *)&fileBuffer.front(), (int)rSize, WEBMVCPP_SENDDATA_FLAGS);
                    ::send(socketDescriptor, "\r\n", 2, WEBMVCPP_SENDDATA_FLAGS);
                    
                    rPos += rSize;
                }
            }
        }
        
        void
        send_content(std::ifstream & fs)
        {
            std::vector<unsigned char> fileBuffer;
            fileBuffer.resize(64 * 1024);
            
            fs.seekg(0, fs.end);
            std::streamoff length = fs.tellg();
            fs.seekg(0, fs.beg);
            
            std::streamoff rPos = 0;
            while (rPos != length)
            {
                std::streamoff rSize = min(64 * 1024, length - rPos);
                fs.read((char *)&fileBuffer.front(), rSize);
                
                std::ostringstream hexStrm;
                hexStrm << std::hex << rSize << "\r\n";
                std::string hexStr = hexStrm.str();
                
                ::send(socketDescriptor, hexStr.c_str(), hexStr.length(), WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, (const char *)&fileBuffer.front(), (int)rSize, WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, "\r\n", 2, WEBMVCPP_SENDDATA_FLAGS);
                
                rPos += rSize;
            }
        }
        
        void
        send_content(const std::vector<unsigned char> & bytes)
        {
            int cPtr = 0;
            unsigned long bufferLength = bytes.size();
            
            while (bufferLength != cPtr)
            {
                unsigned long cFragmentLength = min(64 * 1024, bufferLength - cPtr);
                
                std::ostringstream hexStrm;
                hexStrm << std::hex << cFragmentLength << "\r\n";
                std::string hexStr = hexStrm.str();
                
                ::send(socketDescriptor, hexStr.c_str(), hexStr.length(), WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, (const char *)&bytes.front() + cPtr, cFragmentLength, WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, "\r\n", 2, WEBMVCPP_SENDDATA_FLAGS);
                
                cPtr += cFragmentLength;
            }
        }
        
        void
        send_content(const std::string & text)
        {
            int cPtr = 0;
            size_t bufferLength = text.length();
            
            while (bufferLength != cPtr)
            {
                unsigned long cFragmentLength = min(64 * 1024, bufferLength - cPtr);
                
                std::ostringstream hexStrm;
                hexStrm << std::hex << cFragmentLength << "\r\n";
                std::string hexStr = hexStrm.str();
                
                ::send(socketDescriptor, hexStr.c_str(), hexStr.length(), WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, (const char *)text.c_str() + cPtr, cFragmentLength, WEBMVCPP_SENDDATA_FLAGS);
                ::send(socketDescriptor, "\r\n", 2, WEBMVCPP_SENDDATA_FLAGS);
                
                cPtr += cFragmentLength;
            }
        }

        int get_status_code()
        {
            int result = 0;
            std::istringstream iStream(this->status);
            iStream >> result;

            return result;
        }

        std::string status;
        std::vector<unsigned char> content;
        std::string contentType;

        http_values header;

        bool isCompressed;
        bool isKeepAlive;
        
        int socketDescriptor;
    };
}
#endif // WEBMVCPP_HTTPRESPONSE_H
