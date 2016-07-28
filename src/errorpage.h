#ifndef WEBMVCPP_ERROR_H
#define WEBMVCPP_ERROR_H

namespace webmvcpp
{
    class http_error
    {
        http_error();
        http_error(http_error &);
        void operator=(http_error &);
    public:
        http_error(unsigned short error, const std::string & capt, const std::string & descr):
        errorCode(error),
        caption(capt),
        description(descr)
        {}

		void fill_response(http_response & response)
		{
			std::ostringstream pageContent;

			pageContent << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" \
				"<html xmlns=\"http://www.w3.org/1999/xhtml\">" \
				"<head>" \
				"  <title></title>" \
				"  <style>" \
				"  *{margin:0;padding:0;color: #424242;}" \
				"  .errblock {padding: 25px; overflow: hidden;}" \
				"  .errlogo {width: 45px; height: 55px;line-height:0px;border: 1px solid #424242; float: left;}" \
				"  .errmsg {margin-left: 80px;margin:top: 20px; min-height: 30px;}" \
				"  .wpix{background-color: white;width: 5px;height: 5px; display:inline-block;}" \
				"  .bpix{background-color: #424242;width: 5px;height: 5px; display:inline-block;}" \
				"  .productmsg {padding:10px;}" \
				"  </style>" \
				"</head>" \
				"<body>" \
				"<div class='errblock'>" \
				"  <div class='errlogo'>" \
				"    <p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='wpix'></p><p class='bpix'></p>" \
				"    <p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p><p class='bpix'></p>" \
				"  </div>" \
				"  <div class='errmsg'>";
			pageContent << "    <h2>" << errorCode << " " << caption << "</h2>";
			pageContent << "    <h4>" + description + "</h4>";
			pageContent << "  </div>" \
				"</div>" \
				"<hr width='300px;' />" \
				"<div class='productmsg'>" \
				"<h5>WebMVCpp - Your C++ MVC Web Engine</h5>" \
				"</div>" \
				"</body>" \
				"</html>";

			std::ostringstream httpStatus;
			httpStatus << errorCode << " " << caption;
			response.status = httpStatus.str();

			response.contentType = "text/html";

			unsigned int conntentLength = pageContent.str().length();
			response.content.resize(conntentLength);
			memcpy(&response.content.front(), pageContent.str().c_str(), conntentLength);
		}

    private:
        unsigned short errorCode;
        std::string caption;
        std::string description;
    };
}

#endif // WEBMVCPP_ERROR_H
