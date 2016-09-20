#ifndef WEBMVCPP_SERVER_CONNECTION_H
#define WEBMVCPP_SERVER_CONNECTION_H

namespace webmvcpp
{
    class http_server_connection : public http_connection
    {
        http_server_connection();
    public:
		http_server_connection(core_prototype *c, int socket):
                http_connection(socket),
		httpReqParser(request),
		mvcCore(c)
		{

		}

		void
		run()
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
				catch (...)
				{
					request.isKeepAlive = false;
					const char *fatalErrorMessage = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n<h3>Internal server error</h3><p>WebMVCpp - Your C++ MVC Web Engine</p>";
					::send(socketDescriptor, fatalErrorMessage, strlen(fatalErrorMessage), sendDataFlags);
					break;
				}
			} while (request.isKeepAlive);

#ifdef _WIN32
			::closesocket(socketDescriptor);
#else
			::close(socketDescriptor);
#endif
		}

                bool
		wait_for_header()
		{
			while (!httpReqParser.is_header_received())
			{
				bool readyRead = this->wait_for_data();
				if (!readyRead)
					return false;

				int rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), recvDataFlags);
				if (rcvBytes == 0 || rcvBytes == -1)
					return false;

				httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
			}

			return true;
		}

		bool
		wait_for_content()
		{
			while (!httpReqParser.is_body_received())
			{
				bool readyRead = this->wait_for_data();
				if (!readyRead)
					return false;

				int rcvBytes = ::recv(socketDescriptor, (char *)&recvBuffer.front(), recvBuffer.size(), recvDataFlags);
				if (rcvBytes == 0 || rcvBytes == -1)
					return false;

				httpReqParser.accept_data(&recvBuffer.front(), rcvBytes);
			}

			return true;
		}

	core_prototype *mvc_core() { return mvcCore; }

    private:
        http_request request;
        http_response response;

        http_request_parser httpReqParser;

	core_prototype *mvcCore;
    };
}

#endif // WEBMVCPP_SERVER_CONNECTION_H
