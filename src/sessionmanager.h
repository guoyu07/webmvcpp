#ifndef WEBMVCPP_SESSIONMANAGER_H
#define WEBMVCPP_SESSIONMANAGER_H

namespace webmvcpp
{
    struct session_impl : public webmvcobject
	{
    private:
        session_impl();
    public:
        session_impl(const std::string & key):
        webmvcobject(),
		createTime(std::time(0)),
		lastReqTime(std::time(0))
		{
            data["id"]->set(key);
        }

		variant_map & get_data() {return data;}

		std::mutex & get_lock(){return lock;}

		std::time_t & get_last_req_time() {return lastReqTime;}

		void update_last_req_time(){ lastReqTime = std::time(0); }

        variant operator[](const char *idx)
        {
            return data[idx];
        }

        const variant operator[](const char *idx) const
        {
            return data[idx];
        }

	private:
		std::mutex lock;

		variant_map data;
		std::time_t createTime;
		std::time_t lastReqTime;
	};
    
    typedef boost::intrusive_ptr<session_impl> session;

    class requests_session_manager : public webmvcobject
	{
	public:
        explicit requests_session_manager();

		bool is_enabled() {return enabled;}
		unsigned long count();
		void enable(unsigned int session_inactive_life_time);

		std::string make_cookie_value(const std::string & session_id);
		std::string from_cookie_value(const std::string & cookie);

		void delete_session(variant_map & s);

		session get_session(const std::string & key);
		session create_session(const std::string & key);
		std::string create_session_id();

		void clean_old_sessions();
	protected:
		void timerEvent();

	private:

		bool enabled;
		unsigned int sessionActiveLifeHours;

		unsigned int sessionsCount;

		unsigned long nSession;

		std::mutex lock;
		std::map<std::string, session> sessions;
	};
}

#endif // WEBMVCPP_SESSIONMANAGER_H
