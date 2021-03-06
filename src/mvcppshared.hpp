
using namespace nlohmann;

namespace webmvcpp
{
    class engine;
    struct http_connection_context;
    class http_server;
    class session_manager;

    class mime_file_types_prototype {
        public:
            virtual const std::string get_mime_type(const std::string & filePath) = 0;
        };

    class core_prototype {
    public:
        virtual bool process_request(http_connection_context *ctx) = 0;
        virtual session_manager *get_session_manager() = 0;
        virtual mime_file_types_prototype *get_mime_types() = 0;
        virtual void log(const std::string &logMessage) = 0;
    };

    class http_server_prototype {
    public:
        virtual void release_connection(http_connection_context *ctx) = 0;
        virtual bool is_connection_permitted(unsigned long ipAddress) = 0;
    };
}

#define $(c) make_##c
