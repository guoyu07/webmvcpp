#ifndef WEBMVCPP_UTILS_H
#define WEBMVCPP_UTILS_H

namespace webmvcpp
{
    struct utils {
        static std::string to_upper(const std::string & str)
        {
            std::string resultStr = str;

            std::transform(resultStr.begin(), resultStr.end(), resultStr.begin(), ::toupper);

            return resultStr;
        }

        static std::string to_lower(const std::string & str)
        {
            std::string resultStr = str;

            std::transform(resultStr.begin(), resultStr.end(), resultStr.begin(), ::tolower);

            return resultStr;
        }

        static std::string get_parent_directory(const std::string & path) {
            size_t slpos = path.rfind("\\");
            size_t bslpos = path.rfind("/");
            if (bslpos == std::string::npos)
                return std::string(path.c_str(), bslpos);
            else if (slpos == std::string::npos)
                return std::string(path.c_str(), slpos);
            else
                return "";
        }


        static void append_string(std::vector<unsigned char> & buffer, const std::string & str)
        {
            unsigned int wptr = buffer.size();
            unsigned int strLength = str.length();
            buffer.resize(wptr + strLength);

            memcpy(&buffer.front() + wptr, str.c_str(), strLength);
        }

        static void append_bytes(std::vector<unsigned char> & buffer, const unsigned char *bytes, unsigned int bytesSize)
        {
            unsigned int wptr = buffer.size();
            buffer.resize(wptr + bytesSize);

            memcpy(&buffer.front() + wptr, bytes, bytesSize);
        }

        static void append_bytes(std::vector<unsigned char> & buffer, const std::vector<unsigned char> & bytes)
        {
            append_bytes(buffer, &bytes.front(), bytes.size());
        }

        static std::vector<std::string> split_string(const std::string & str, const char separator)
        {
            std::vector<std::string> strings;

            std::istringstream strstrm(str);

            std::string line;
            while (std::getline(strstrm, line, separator))
            {
                strings.push_back(line);
            }

            return strings;
        }

        static std::string replace_string(std::string & content, const std::string & from, const std::string & to)
        {
            std::string result;

            size_t prevPos = 0;
            size_t curPos = 0;
            while ((curPos = content.find(from, curPos)) != std::string::npos)
            {
                result += content.substr(prevPos, curPos - prevPos);
                curPos += from.length();

                result += to;

                prevPos = curPos;
            }

            unsigned int contentLength = content.length();
            if (prevPos != contentLength) {
                result += content.substr(prevPos, contentLength - prevPos);
            }

            return result;
        }

        static std::string
        multiply_replace_string(std::string & content, const std::string & fragmentFrom1, const std::string & fragmentFrom2, const std::string & fragmentTo1, const std::string & fragmentTo2)
        {
            std::string result;

            size_t prevPos = 0;
            size_t curPos = 0;
            while ((curPos = content.find(fragmentFrom1, prevPos)) != std::string::npos)
            {
                result += content.substr(prevPos, curPos - prevPos);
                curPos += fragmentFrom1.length();

                size_t endBlockPos = content.find(fragmentFrom2, curPos);
                if (endBlockPos == std::string::npos)
                    break;

                result += fragmentTo1 + content.substr(curPos, endBlockPos - curPos) + fragmentTo2;

                endBlockPos += fragmentFrom2.length();
                prevPos = endBlockPos;
            }

            unsigned int ctrlTempLength = content.length();
            if (prevPos != ctrlTempLength) {
                result += content.substr(prevPos, ctrlTempLength - prevPos);
            }
                

            return result;
        }

        static std::string
        multiply_replace_string(const std::string & content, const std::string & beginBlock, const std::string & endBlock, const std::map<std::string, std::string> & values)
        {
            std::string result;

            size_t curPos = 0;
            size_t prevPos = 0;
            while ((curPos = content.find(beginBlock, curPos)) != std::string::npos)
            {
                result += content.substr(prevPos, curPos - prevPos);

                curPos += beginBlock.length();

                size_t endBlockPos = content.find(endBlock, curPos);
                if (endBlockPos == std::string::npos)
                    break;

                std::string blockName = content.substr(curPos, endBlockPos - curPos);

                endBlockPos += endBlock.length();

                curPos = endBlockPos;
                prevPos = curPos;

                std::map<std::string, std::string>::const_iterator it = values.find(blockName);
                if (it != values.end())
                    result += it->second;
            }

            unsigned int ctrlTempLength = content.length();
            if (prevPos != ctrlTempLength)
                result += content.substr(prevPos, ctrlTempLength - prevPos);

            return result;
        }


        static std::string to_rfc2822_datetime(time_t & timeValue)
        {
            const char * _toDaysOfWeek3[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
            const char * _toMonth3[] = { "Jan", "Fev", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

            tm * ti = localtime(&timeValue);

            std::ostringstream rfc2822time;
            rfc2822time << _toDaysOfWeek3[ti->tm_wday] << ", " << ti->tm_mday << " " << _toMonth3[ti->tm_mon] << " " << (ti->tm_year + 1900) << " " << std::setw(2) << ti->tm_hour << ":" << ti->tm_min << ":" << ti->tm_sec << " GMT";

            return rfc2822time.str();
        }

        static std::string trim_string(const std::string & str)
        {
            auto wsfront = std::find_if_not(str.begin(), str.end(), [](int c) {return std::isspace(c); });
            auto wsback = std::find_if_not(str.rbegin(), str.rend(), [](int c) {return std::isspace(c); }).base();
            return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
        }

        static std::string reduce_string(const std::string & str)
        {
            std::string result = trim_string(str);

            const std::string fill = " ";
            const std::string whitespace = "  ";

            // replace sub ranges
            size_t beginSpace = result.find_first_of(whitespace);
            while (beginSpace != std::string::npos)
            {
                const size_t endSpace = result.find_first_not_of(whitespace, beginSpace);
                const size_t range = endSpace - beginSpace;

                result.replace(beginSpace, range, fill);

                const size_t newStart = beginSpace + fill.length();
                beginSpace = result.find_first_of(whitespace, newStart);
            }

            return result;
        }

        static inline bool is_base64(unsigned char c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }

        static std::string base64_encode(const unsigned char *bytesToEncode, unsigned int len)
        {
            static const std::string base64_chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

            std::string ret;
            int i = 0;
            int j = 0;
            unsigned char char_array_3[3];
            unsigned char char_array_4[4];

            while (len--) {
                char_array_3[i++] = *(bytesToEncode++);
                if (i == 3) {
                    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                    char_array_4[3] = char_array_3[2] & 0x3f;

                    for (i = 0; (i <4); i++)
                        ret += base64_chars[char_array_4[i]];
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 3; j++)
                    char_array_3[j] = '\0';

                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (j = 0; (j < i + 1); j++)
                    ret += base64_chars[char_array_4[j]];

                while ((i++ < 3))
                    ret += '=';

            }

            return ret;

        }

        static std::string base64_decode(const std::string & encodedString)
        {
            const std::string base64_chars =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";

            int inLen = encodedString.size();
            int i = 0;
            int j = 0;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            std::string ret;

            while (inLen-- && (encodedString[in_] != '=') && is_base64(encodedString[in_])) {
                char_array_4[i++] = encodedString[in_]; in_++;
                if (i == 4) {
                    for (i = 0; i <4; i++)
                        char_array_4[i] = (unsigned char)base64_chars.find(char_array_4[i]);

                    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                    for (i = 0; (i < 3); i++)
                        ret += char_array_3[i];
                    i = 0;
                }
            }

            if (i) {
                for (j = i; j <4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j <4; j++)
                    char_array_4[j] = (unsigned char)base64_chars.find(char_array_4[j]);

                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

                for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
            }

            return ret;
        }

        static inline unsigned char from_hex(unsigned char ch)
        {
            if (ch <= '9' && ch >= '0')
                ch -= '0';
            else if (ch <= 'f' && ch >= 'a')
                ch -= 'a' - 10;
            else if (ch <= 'F' && ch >= 'A')
                ch -= 'A' - 10;
            else
                ch = 0;
            return ch;
        }


        static inline unsigned char to_hex(unsigned char x)
        {
            return x + (x > 9 ? ('A' - 10) : '0');
        }


        static std::string decode_uri(const std::string & encoded)
        {
            using namespace std;
            string result;
            string::size_type i;
            for (i = 0; i < encoded.size(); ++i)
            {
                if (encoded[i] == '+')
                {
                    result += ' ';
                }
                else if (encoded[i] == '%' && encoded.size() > i + 2)
                {
                    const unsigned char ch1 = from_hex(encoded[i + 1]);
                    const unsigned char ch2 = from_hex(encoded[i + 2]);
                    const unsigned char ch = (ch1 << 4) | ch2;
                    result += ch;
                    i += 2;
                }
                else
                {
                    result += encoded[i];
                }
            }
            return result;
        }

        static const std::string urlencode(const std::string & decoded)
        {
            std::ostringstream os;

            for (std::string::const_iterator ci = decoded.begin(); ci != decoded.end(); ++ci)
            {
                if ((*ci >= 'a' && *ci <= 'z') ||
                    (*ci >= 'A' && *ci <= 'Z') ||
                    (*ci >= '0' && *ci <= '9'))
                {
                    os << *ci;
                }
                else if (*ci == ' ')
                {
                    os << '+';
                }
                else
                {
                    os << '%' << to_hex(*ci >> 4) << to_hex(*ci % 16);
                }
            }

            return os.str();
        }

        static const std::string to_cHexString(const std::string & str)
        {
            std::ostringstream os;

            os << "\"";

            unsigned long numByte = 0;

            for (std::string::const_iterator ci = str.begin(); ci != str.end(); ++ci) {
                unsigned char oneChar = *ci;
                os << "\\x" << to_hex(oneChar / 16) << to_hex(oneChar % 16);
                ++numByte %= 32;
                if (numByte == 31) {
                    os << "\" \\" << std::endl << "    \"";
                }
            }

            os << "\"";

            return os.str();
        }
    };
}

#endif // WEBMVCPP_UTILS_H