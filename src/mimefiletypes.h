#ifndef WEBMVCPP_MIMETYPES_H
#define WEBMVCPP_MIMETYPES_H

#include <string>
#include <map>

namespace webmvcpp
{
    struct mime_file_types
    {
    public:
        const std::string get_mime_type(const std::string & file_path);

		static std::map<std::string, std::string> mimeTypes;
    };
}

#endif // WEBMVCPP_MIMETYPES_H
