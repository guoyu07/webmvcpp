#include "webmvcpp.h"

namespace webmvcpp
{
	requests_session_manager::requests_session_manager():
	webmvcobject(),
	enabled(false),
	sessionActiveLifeHours(0),
	sessionsCount(0),
	nSession(0)
	{
	}

	unsigned long
	requests_session_manager::count()
	{
		std::lock_guard<std::mutex> locker(lock);

		return sessions.size();
	}


	session
	requests_session_manager::get_session(const std::string & key)
	{
		std::lock_guard<std::mutex> locker(lock);

		std::map<std::string, session>::iterator it = sessions.find(key);
		if (it == sessions.end())
			return NULL;

		return it->second;
	}

	session
	requests_session_manager::create_session(const std::string & key)
	{
		std::lock_guard<std::mutex> locker(lock);

        session newSession = new session_impl(key);
		sessions.insert(std::pair<std::string, session>(key, newSession));

		return newSession;
	}

	std::string
	requests_session_manager::create_session_id()
	{
		std::ostringstream randopmBuffer;
		randopmBuffer << std::time(0) << "__" << ++nSession;
		return  md5(randopmBuffer.str());
	}

	void
	requests_session_manager::clean_old_sessions()
	{
		std::lock_guard<std::mutex> locker(lock);

		time_t currentDateTime = std::time(0);

		std::map<std::string, session>::iterator it = sessions.begin();
		while(it!=sessions.end())
		{
			if ((it->second->get_last_req_time() + 3600 * sessionActiveLifeHours) < currentDateTime)
				it = sessions.erase(it);
			else
				++it;
		}
	}

	void
	requests_session_manager::enable(unsigned int aliveHours)
	{
		if (enabled)
			return;

		enabled = true;

		sessionActiveLifeHours = aliveHours;

		///
	}

	std::string
	requests_session_manager::from_cookie_value(const std::string & cookie)
	{
		std::vector<std::string> cookieValues = split_string(cookie, ';');
		for (std::vector<std::string>::iterator it = cookieValues.begin();it!=cookieValues.end();++it)
		{
			std::string keyValuePair = trim_string(*it);

			std::vector<std::string> keyValueSplitted = split_string(keyValuePair, '=');
			if (keyValueSplitted.size()==2 && keyValueSplitted.front()=="WMVCPPID")
				return keyValueSplitted.back();
		}
		return std::string();
	}

	std::string
	requests_session_manager::make_cookie_value(const std::string & sessionId)
	{
		time_t expiredDate = std::time(0) + 3600 * sessionActiveLifeHours;
		std::string gmtExpiredDate = to_rfc2822_datetime(expiredDate);

		std::ostringstream cookieValue;
		cookieValue << "WMVCPPID=" << sessionId << "; expires=" << gmtExpiredDate << "; path=/";

		return cookieValue.str();
	}


	void
		requests_session_manager::delete_session(variant_map & s)
	{
		variant_map::iterator it = s.find("id");
		if (it == s.end())
			return;

		std::lock_guard<std::mutex> locker(lock);

        std::map<std::string, session>::iterator sessionIt = sessions.find(it->second->to_string());
        if (sessionIt != sessions.end())
            sessions.erase(sessionIt);
	}
}