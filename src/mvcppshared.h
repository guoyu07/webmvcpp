
using namespace nlohmann;

namespace webmvcpp
{
	class core;
	class http_server;
	class http_connection;
	class session_manager;

	class core_prototype {
	public:
		virtual bool process_request(http_connection *connection, http_request & request, http_response & response) = 0;
		virtual session_manager *get_session_manager() = 0;
		virtual mime_file_types *get_mime_types() = 0;
	};
}

#define $(c) make_##c