#ifndef WEBMVCPP_ERROR_H
#define WEBMVCPP_ERROR_H

namespace webmvcpp
{
    struct error_page
    {
        static void send(http_response & response, const unsigned int httpCode, const std::string & caption, const std::string & description)
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
            pageContent << "    <h2>" << httpCode << " " << caption << "</h2>";
            pageContent << "    <h4>" + description + "</h4>";
            pageContent << "  </div>" \
                "</div>" \
                "<hr width='300px;' />" \
                "<div class='productmsg'>" \
                "<h5>WebMVC++ Open Source Web Application Engine</h5>" \
                "<h5>Version: " << WEBMVCPP_MAJOR_VERSION << "." << WEBMVCPP_MINOR_VERSION << "." << WEBMVCPP_BUILD_NUMBER << "</h5>" \
                "</div>" \
                "</body>" \
                "</html>";

            std::ostringstream httpStatus;
            httpStatus << httpCode << " " << caption;
            response.status = httpStatus.str();

            response.contentType = "text/html";
            response.send_header();
            response.send_content(pageContent.str());
            response.end();
        }
    };
}

#endif // WEBMVCPP_ERROR_H
