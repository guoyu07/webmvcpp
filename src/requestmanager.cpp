#include "webmvcpp.h"

namespace webmvcpp
{

    request_manager::request_manager(core *coreApp):
    webMvcCore(coreApp)
	{
    
    }

	void
    request_manager::send_static_file(application *webapp, http_connection connection, http_request & request, http_response & response)
	{
        std::string filePath = webapp->staticPath + request.path;

		if (filePath.find("..") != std::string::npos)
		{
			response.status = "400 Bad request";
			response.contentType = "text/html";

			connection->send_response_header(response);
			connection->send_response_content("<h2>Bad request</h2>");
			return;
		}

		std::ifstream fs(filePath, std::ios::in | std::ios::binary);
		if (!fs.is_open())
		{
			response.status = "404 Not found";
			response.contentType = "text/html";

			connection->send_response_header(response);
			connection->send_response_content("<h2>404 Not Found</h2>");

			return;
		}

        response.contentType = webMvcCore->mimeTypes().get_mime_type(request.path);

		if (request.rangesExist)
		{
			fs.seekg(0, std::ios::end);
			int64_t fileSize = fs.tellg();
			fs.seekg(0, std::ios::beg);

			std::ostringstream contentRangeAnswer;

			std::vector<std::pair<int64_t, int64_t> >::iterator rangesIt = request.ranges.begin();
			while (rangesIt != request.ranges.end())
			{
				std::pair<int64_t, int64_t> & range = *rangesIt;

				if (range.first == -1)
				{
					range.first = fileSize - range.second;
					range.second = fileSize;
				}
				else if (range.second == -1 || range.second > fileSize)
				{
					range.second = fileSize;
				}

				if (range.second - range.first == 0 || range.first > fileSize)
				{
					request.ranges.erase(rangesIt);
				}
				else
				{
					if (rangesIt != request.ranges.begin())
						contentRangeAnswer << ",";

					contentRangeAnswer << range.first << "-" << range.second;

					++rangesIt;
				}
			}

			if (request.ranges.size() == 0)
			{
				response.status = "416 Requested range not satisfiable";
				response.contentType = "text/html";

				connection->send_response_header(response);
				connection->send_response_content("<h2>Range is wrong</h2>");

				return;
			}

			response.status = "206 Partial Content";

			contentRangeAnswer << "/" << fileSize;

			response.header.insert(std::pair<std::string, std::string>("Content-Range", contentRangeAnswer.str()));
		}

		time_t expiredDate = std::time(0);
		expiredDate += 1 * 60 * 60 * 24 * 145;
		response.header.insert(std::pair<std::string, std::string>("Expires", to_rfc2822_datetime(expiredDate)));

		connection->send_response_header(response);

		if (request.rangesExist)
			connection->send_response_content(fs, request.ranges);
		else
			connection->send_response_content(fs);
	}

	bool
    request_manager::is_model_valid(application *mvcapp, http_connection connection, http_request & request, session sessionContext, const std::map<std::string, request_model> & m)
	{
		std::map<std::string, request_model>::const_iterator it = m.find(request.method);
		if (it == m.end())
			return true;

		const std::set<std::string> & flags = it->second.flags;

		for (std::set<std::string>::const_iterator flagsIt = flags.cbegin(); flagsIt != flags.cend(); ++flagsIt)
		{
			const std::string & flag = *flagsIt;

			if (flag == MVCPP_MODEL_FLAGS_AUTHORIZED)
			{
				std::lock_guard<std::mutex> sessionDataLocker(sessionContext->get_lock());

                if (!mvcapp->handlers->checkAuthorized || !mvcapp->handlers->checkAuthorized(request, sessionContext->get_data()))
					return false;
			}
		}

		if (request.method == "GET")
		{
			const std::list<std::string> & queryString = it->second.queryString;
			for (std::list<std::string>::const_iterator argIt = queryString.cbegin(); argIt != queryString.cend(); ++argIt)
			{
				const std::map<std::string, std::string> & getParams = request.getParams;
				if (getParams.find(*argIt) == getParams.end())
					return false;
			}

			return true;
		}
		else if (request.method == "POST")
		{
			const std::string & contentType = it->second.contentType;

			if (contentType.length() != 0 && request.contentType.find(contentType) == std::string::npos)
				return false;

			const std::list<std::string> & queryString = it->second.queryString;
			for (std::list<std::string>::const_iterator argIt = queryString.cbegin(); argIt != queryString.cend(); ++argIt)
			{
				const std::map<std::string, std::string> & getParams = request.getParams;
				if (getParams.find(*argIt) == getParams.end())
					return false;
			}

			const std::list<std::string> & bodyUrlEncodedParams = it->second.bodyUrlEncodedParams;
			for (std::list<std::string>::const_iterator argIt = bodyUrlEncodedParams.cbegin(); argIt != bodyUrlEncodedParams.cend(); ++argIt)
			{
				const std::map<std::string, std::string> & postParams = request.postParams;
				if (postParams.find(*argIt) == postParams.end())
					return false;
			}

			return true;
		}

		return true;
	}

    void
    request_manager::send_mvc_page(application *mvcapp, http_connection connection, http_request & request, http_response & response)
	{
		response.contentType = "text/html";

        requests_session_manager & sessionManager = connection->mvc_core()->session_manager();

		mvc_view_data viewData;

		std::string sessionId;

		session sessionContext;

        if (sessionManager.is_enabled())
		{
            http_values::const_iterator itHeadeer = request.header.find("Cookie");
			if (itHeadeer != request.header.end())
                sessionId = sessionManager.from_cookie_value(itHeadeer->second);

            if (sessionId.length() == 0 || !(sessionContext = sessionManager.get_session(sessionId)))
			{
                sessionId = sessionManager.create_session_id();
                sessionContext = sessionManager.create_session(sessionId);

                if (mvcapp->handlers->createSession)
				{
					std::lock_guard<std::mutex> locker(sessionContext->get_lock());
                    mvcapp->handlers->createSession(request, sessionContext->get_data());
				}
			}

            response.header.insert(std::pair<std::string, std::string>("Set-Cookie", sessionManager.make_cookie_value(sessionId)));
		}

        std::map<std::string, std::map<std::string, request_model>> & reqModels = mvcapp->reqModels;
		std::map<std::string, std::map<std::string, request_model> >::const_iterator reqModelIt = reqModels.find(request.path);
		if (reqModelIt == reqModels.cend())
		{
			if (request.method == "POST")
				connection->wait_for_content();
		}
		else
		{
			std::map<std::string, request_model> reqModel = reqModelIt->second;

			if (request.method == "POST")
			{
				std::map<std::string, request_model>::const_iterator postReqModel = reqModel.find("POST");
				if (postReqModel != reqModel.end())
				{
					const std::set<std::string> & flags = postReqModel->second.flags;
					if (flags.find(MVCPP_MODEL_FLAGS_STREAM) == flags.cend())
						connection->wait_for_content();
				}

			}

            if (!is_model_valid(mvcapp, connection, request, sessionContext, reqModelIt->second))
			{
				response.status = "400 Bad Request";
				append_string(response.content, response.status);

				connection->send_response_header(response);
				connection->send_response_content(response.content);

				return;
			}

		}

        std::map<std::string, webmvcpp_request_handler> & req = mvcapp->handlers->requests;
        std::map<std::string, webmvcpp_request_handler>::const_iterator reqHandlerIt = req.find(request.path);
        if (reqHandlerIt != req.end())
		{
			std::lock_guard<std::mutex> locker(sessionContext->get_lock());

			if (reqHandlerIt->second(connection, request, response, sessionContext->get_data()))
			{
				connection->send_response_header(response);
				connection->send_response_content(response.content);

				return;
			}
		}

        std::map<std::string, std::string> & pages = mvcapp->pages;
		std::map<std::string, std::string>::iterator pageIt = pages.find(request.path);
        if (pageIt != pages.end())
		{
			std::string pageContent = pageIt->second;

            if (mvcapp->handlers->masterPageHandler)
			{
	        	std::lock_guard<std::mutex> locker(sessionContext->get_lock());

                mvcapp->handlers->masterPageHandler(connection, request, response, sessionContext->get_data(), viewData);
			}

            std::map<std::string, webmvcpp_view_handler> views = mvcapp->handlers->views;
            std::map<std::string, webmvcpp_view_handler>::const_iterator contentHandlerIt = views.find(request.path);
            if (contentHandlerIt != views.end())
			{
				std::lock_guard<std::mutex> locker(sessionContext->get_lock());

				contentHandlerIt->second(connection, request, response, sessionContext->get_data(), viewData);
			}

			pageContent = multiply_replace_string(pageContent, "<webmvcpp:viewdata:", " />", viewData);

			connection->send_response_header(response);
			connection->send_response_content(pageContent);

			return;
		}

		response.status = "404 Not found";
		response.contentType = "text/html";

		connection->send_response_header(response);
		connection->send_response_content("<h2>404 Not Found</h2>");
	}

	void
    request_manager::process_request(application *mvcapp, http_connection connection, http_request & request, http_response & response)
	{
		response.status = "200 OK";

        std::map<std::string, std::string> & routeMap = mvcapp->routeMap;
		std::map<std::string, std::string>::const_iterator routeIt = routeMap.find(request.path);
		if (routeIt != routeMap.cend())
			request.path = routeIt->second;

		std::vector<std::string> splittedPath = split_string(request.path, '/');

		std::string controllerName = splittedPath.size()>2 ? splittedPath[1] : "";

        std::set<std::string> & controllers = mvcapp->controllers;
		if (splittedPath.size()>2 && controllers.find(controllerName) != controllers.end())
		{
			send_mvc_page(mvcapp, connection, request, response);
		}
		else
            send_static_file(mvcapp, connection, request, response);

		connection->end_response();
	}
}