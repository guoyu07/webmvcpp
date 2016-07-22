#ifndef WEBMVCPP_UTILS_H
#define WEBMVCPP_UTILS_H

namespace webmvcpp
{
    std::string to_upper(const std::string & str);
    std::string to_lower(const std::string & str);

	std::string get_parent_directory(const std::string & path);

    void append_string(std::vector<unsigned char> & buffer, const std::string & str);

    void append_bytes(std::vector<unsigned char> & buffer, const std::vector<unsigned char> & bytes);

    void append_bytes(std::vector<unsigned char> & buffer, const unsigned char *bytes, unsigned int bytesSize);

	std::vector<std::string> split_string(const std::string & str, const char separator);

    std::string multiply_replace_string(const std::string & content, const std::string & beginBlock, const std::string & endBlock, const std::map<std::string, std::string> & values);

	std::string to_rfc2822_datetime(time_t & timeValue);

	std::string trim_string(const std::string & str);

    std::string reduce_string(const std::string & str);

    std::string base64_decode(const std::string & encodedString);
    std::string base64_encode(const unsigned char *bytesToEncode, unsigned int len);

    std::string decode_uri(const std::string & encoded);
}

#endif // WEBMVCPP_UTILS_H