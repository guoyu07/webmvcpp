
using namespace nlohmann;

namespace webmvcpp
{
    class engine;
    class http_server;
    class http_incoming_connection;
    class session_manager;

    class mime_file_types_prototype {
        public:
            virtual const std::string get_mime_type(const std::string & filePath) = 0;
        };

    class core_prototype {
    public:
        virtual bool process_request(http_incoming_connection *connection, http_request & request, http_response & response) = 0;
        virtual session_manager *get_session_manager() = 0;
        virtual mime_file_types_prototype *get_mime_types() = 0;
        virtual void log(const std::string &logMessage) = 0;
    };

    class http_server_prototype {
    public:
        virtual void release_connection(unsigned long ipAddress, http_incoming_connection *t) = 0;
        virtual bool is_connection_permitted(unsigned long ipAddress) = 0;
    };
}

#define $(c) make_##c
