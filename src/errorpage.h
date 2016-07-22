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

        void fill_response(http_response & response);

    private:
        unsigned short errorCode;
        std::string caption;
        std::string description;
    };
}

#endif // WEBMVCPP_ERROR_H
