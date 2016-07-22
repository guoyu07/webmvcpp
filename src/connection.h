#ifndef WEBMVCPP_CONNECTION_H
#define WEBMVCPP_CONNECTION_H

namespace webmvcpp
{
    class core;

    class http_connection_impl : public webmvcobject

    {
        http_connection_impl();
    public:
        http_connection_impl(core *c, int socket);

        void run();

        void end_response();

        bool wait_for_data();
        bool read_stream(std::vector<unsigned char> & streamData);
        
        bool wait_for_header();
        bool wait_for_content();

        void send_response_header(const http_response & response);
        void send_response_content(const std::string & text);
        void send_response_content(std::ifstream & fs);
        void send_response_content(std::ifstream & fs, const std::vector<std::pair<int64_t, int64_t> > & ranges);
        void send_response_content(const std::vector<unsigned char> & bytes);

        core *mvc_core() { return mvcCore; }

    private:
        void parse_post_url_encoded_params();
        void parse_post_multipartdata_params();

        http_request request;
        http_response response;

        http_request_parser httpReqParser;
        
        int socketDescriptor;
        std::vector<unsigned char> recvBuffer;

        core *mvcCore;
    };

    typedef boost::intrusive_ptr<http_connection_impl> http_connection;
}

#endif // WEBMVCPP_CONNECTION_H
