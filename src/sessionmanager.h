#ifndef WEBMVCPP_SESSIONMANAGER_H
#define WEBMVCPP_SESSIONMANAGER_H

namespace webmvcpp
{
    struct session_impl
	{
    private:
        session_impl();
    public:
        session_impl(const std::string & key):
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

		unsigned long reference = 0;
	private:
		std::mutex lock;

		variant_map data;
		std::time_t createTime;
		std::time_t lastReqTime;
	};
    
    typedef boost::intrusive_ptr<session_impl> session;

    class session_manager
	{
	public:
        explicit session_manager() :
		enabled(false),
		sessionActiveLifeHours(0),
		sessionsCount(0),
		nSession(0)
		{
		}

		bool isEnabled() { return enabled; }

		unsigned long
		count()
		{
			std::lock_guard<std::mutex> locker(lock);

			return sessions.size();
		}


		session
		get_session(const std::string & key)
		{
			std::lock_guard<std::mutex> locker(lock);

			std::map<std::string, session>::iterator it = sessions.find(key);
			if (it == sessions.end())
				return NULL;

			return it->second;
		}

		session
		create_session(const std::string & key)
		{
			std::lock_guard<std::mutex> locker(lock);

			session newSession = new session_impl(key);
			sessions.insert(std::pair<std::string, session>(key, newSession));

			return newSession;
		}

		std::string
		create_session_id()
		{
			std::ostringstream randopmBuffer;
			randopmBuffer << std::time(0) << "__" << ++nSession;
			return  md5(randopmBuffer.str());
		}

		void
		clean_old_sessions()
		{
			std::lock_guard<std::mutex> locker(lock);

			time_t currentDateTime = std::time(0);

			std::map<std::string, session>::iterator it = sessions.begin();
			while (it != sessions.end())
			{
				if ((it->second->get_last_req_time() + 3600 * sessionActiveLifeHours) < currentDateTime)
					it = sessions.erase(it);
				else
					++it;
			}
		}

		void
		enable(unsigned int aliveHours)
		{
			if (enabled)
				return;

			enabled = true;

			sessionActiveLifeHours = aliveHours;

			///
		}

		std::string
		from_cookie_value(const std::string & cookie)
		{
			std::vector<std::string> cookieValues = utils::split_string(cookie, ';');
			for (std::vector<std::string>::iterator it = cookieValues.begin(); it != cookieValues.end(); ++it)
			{
				std::string keyValuePair = utils::trim_string(*it);

				std::vector<std::string> keyValueSplitted = utils::split_string(keyValuePair, '=');
				if (keyValueSplitted.size() == 2 && keyValueSplitted.front() == "WMVCPPID")
					return keyValueSplitted.back();
			}
			return std::string();
		}

		std::string
		make_cookie_value(const std::string & sessionId)
		{
			time_t expiredDate = std::time(0) + 3600 * sessionActiveLifeHours;
			std::string gmtExpiredDate = utils::to_rfc2822_datetime(expiredDate);

			std::ostringstream cookieValue;
			cookieValue << "WMVCPPID=" << sessionId << "; expires=" << gmtExpiredDate << "; path=/";

			return cookieValue.str();
		}

		void
		delete_session(variant_map & s)
		{
			variant_map::iterator it = s.find("id");
			if (it == s.end())
				return;

			std::lock_guard<std::mutex> locker(lock);

			std::map<std::string, session>::iterator sessionIt = sessions.find(it->second->to_string());
			if (sessionIt != sessions.end())
				sessions.erase(sessionIt);
		}

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
