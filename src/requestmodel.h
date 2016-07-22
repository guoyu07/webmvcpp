#ifndef WEBMVCPP_REQMODEL_H
#define WEBMVCPP_REQMODEL_H

namespace webmvcpp
{
    #define MVCPP_MODEL_FLAGS_AUTHORIZED "authorized"
    #define MVCPP_MODEL_FLAGS_STREAM "stream"

    #define MVCPP_MODEL_KEY_FLAGS "flags"
    #define MVCPP_MODEL_KEY_CONTENT_TYPE "content_type"
    #define MVCPP_MODEL_KEY_BODY_ENCODED_PARAMS "body_encoded_params"
    #define MVCPP_MODEL_KEY_QUERY_STRING "query_string"

    class request_model : public webmvcobject
    {
    public:
        request_model(){}

        std::string contentType;

        std::set<std::string> flags;

        std::list<std::string> bodyUrlEncodedParams;
        std::list<std::string> queryString;
    };
}

#endif // WEBMVCPP_REQMODEL_H
