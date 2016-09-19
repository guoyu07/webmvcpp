#include <iostream>
#include <vector>
#include <string>
#include <fstream>

static const char* s_hexTable[256] =
{
    "0x00", "0x01", "0x02", "0x03", "0x04", "0x05", "0x06", "0x07", "0x08", "0x09", "0x0a", "0x0b", "0x0c", "0x0d", "0x0e", "0x0f", "0x10", "0x11",
    "0x12", "0x13", "0x14", "0x15", "0x16", "0x17", "0x18", "0x19", "0x1a", "0x1b", "0x1c", "0x1d", "0x1e", "0x1f", "0x20", "0x21", "0x22", "0x23",
    "0x24", "0x25", "0x26", "0x27", "0x28", "0x29", "0x2a", "0x2b", "0x2c", "0x2d", "0x2e", "0x2f", "0x30", "0x31", "0x32", "0x33", "0x34", "0x35",
    "0x36", "0x37", "0x38", "0x39", "0x3a", "0x3b", "0x3c", "0x3d", "0x3e", "0x3f", "0x40", "0x41", "0x42", "0x43", "0x44", "0x45", "0x46", "0x47",
    "0x48", "0x49", "0x4a", "0x4b", "0x4c", "0x4d", "0x4e", "0x4f", "0x50", "0x51", "0x52", "0x53", "0x54", "0x55", "0x56", "0x57", "0x58", "0x59",
    "0x5a", "0x5b", "0x5c", "0x5d", "0x5e", "0x5f", "0x60", "0x61", "0x62", "0x63", "0x64", "0x65", "0x66", "0x67", "0x68", "0x69", "0x6a", "0x6b",
    "0x6c", "0x6d", "0x6e", "0x6f", "0x70", "0x71", "0x72", "0x73", "0x74", "0x75", "0x76", "0x77", "0x78", "0x79", "0x7a", "0x7b", "0x7c", "0x7d",
    "0x7e", "0x7f", "0x80", "0x81", "0x82", "0x83", "0x84", "0x85", "0x86", "0x87", "0x88", "0x89", "0x8a", "0x8b", "0x8c", "0x8d", "0x8e", "0x8f",
    "0x90", "0x91", "0x92", "0x93", "0x94", "0x95", "0x96", "0x97", "0x98", "0x99", "0x9a", "0x9b", "0x9c", "0x9d", "0x9e", "0x9f", "0xa0", "0xa1",
    "0xa2", "0xa3", "0xa4", "0xa5", "0xa6", "0xa7", "0xa8", "0xa9", "0xaa", "0xab", "0xac", "0xad", "0xae", "0xaf", "0xb0", "0xb1", "0xb2", "0xb3",
    "0xb4", "0xb5", "0xb6", "0xb7", "0xb8", "0xb9", "0xba", "0xbb", "0xbc", "0xbd", "0xbe", "0xbf", "0xc0", "0xc1", "0xc2", "0xc3", "0xc4", "0xc5",
    "0xc6", "0xc7", "0xc8", "0xc9", "0xca", "0xcb", "0xcc", "0xcd", "0xce", "0xcf", "0xd0", "0xd1", "0xd2", "0xd3", "0xd4", "0xd5", "0xd6", "0xd7",
    "0xd8", "0xd9", "0xda", "0xdb", "0xdc", "0xdd", "0xde", "0xdf", "0xe0", "0xe1", "0xe2", "0xe3", "0xe4", "0xe5", "0xe6", "0xe7", "0xe8", "0xe9",
    "0xea", "0xeb", "0xec", "0xed", "0xee", "0xef", "0xf0", "0xf1", "0xf2", "0xf3", "0xf4", "0xf5", "0xf6", "0xf7", "0xf8", "0xf9", "0xfa", "0xfb",
    "0xfc", "0xfd", "0xfe", "0xff"
};

void codeLineToHex(const std::string & codeLine, std::ostream & content)
{
    content << "      ";
    for(unsigned int i = 0;i<codeLine.length();++i)
    {
        unsigned char hexValue = codeLine[i];
        content << s_hexTable[hexValue] << ", ";
    }
    content << "0x0A, ";
    content << " \\" << std::endl;
}

bool appendFileToContent(const char *sourceFilePath, std::ostream & content)
{
    std::cout << "Amalgamation file: " << sourceFilePath << std::endl;

    std::ifstream sourceFile(sourceFilePath);
    if (!sourceFile.is_open()) {
        std::cout << "Error: cant open file: " << sourceFilePath << std::endl;
    	return false;
    }

    std::string codeLine;
    while(std::getline(sourceFile,codeLine)){
      if (codeLine.length() == 0 || (codeLine.length()>10 && codeLine.substr(0, 10)=="#include \"")) {
        content << "0x0D, 0x0A, ";
        content << " \\" << std::endl;
      }
      else {
        codeLineToHex(codeLine, content);
      }
    }

    return true;
}

int main()
{
    std::vector<std::string> sources_cpp;
    sources_cpp.push_back("3rdparty/pugixml/pugixml.cpp");
    sources_cpp.push_back("3rdparty/md5/md5.cpp");

    std::vector<std::string> headers_hpp;
    headers_hpp.push_back("src/declarations.h");
#ifdef _WIN32
	headers_hpp.push_back("3rdparty/dirent_win32.h");
#endif
    headers_hpp.push_back("3rdparty/boost/intrusive_ptr.hpp");
    headers_hpp.push_back("3rdparty/json.hpp");
    headers_hpp.push_back("3rdparty/pugixml/pugixml.hpp");
    headers_hpp.push_back("3rdparty/md5/md5.h");
    headers_hpp.push_back("src/buildinfo.h");
    headers_hpp.push_back("src/utils.h");
    headers_hpp.push_back("src/systemutils.h");
    headers_hpp.push_back("src/mimefiletypes.h");
    headers_hpp.push_back("src/httprequest.h");
    headers_hpp.push_back("src/httpresponse.h");
    headers_hpp.push_back("src/mvcppshared.h");
    headers_hpp.push_back("src/variant.h");
    headers_hpp.push_back("src/errorpage.h");
    headers_hpp.push_back("src/multipartparser.h");
    headers_hpp.push_back("src/handlers.h");
    headers_hpp.push_back("src/requestmodel.h");
    headers_hpp.push_back("src/webapplication.h");
    headers_hpp.push_back("src/requestparser.h");
    headers_hpp.push_back("src/sessionmanager.h");
    headers_hpp.push_back("src/connection.h");
    headers_hpp.push_back("src/requestmanager.h");
    headers_hpp.push_back("src/applicationloader.h");
    headers_hpp.push_back("src/connectionthread.h");
    headers_hpp.push_back("src/server.h");
    headers_hpp.push_back("src/builder.h");
    headers_hpp.push_back("src/webmvcppcore.h");

    std::vector<std::string> http_parser_headers_h;
    http_parser_headers_h.push_back("3rdparty/http_parser/http_parser.h");

    std::vector<std::string> multipart_parser_headers_h;
    multipart_parser_headers_h.push_back("3rdparty/multipart_parser/multipart_parser.h");

    std::vector<std::string> http_parser_sources_c;
    http_parser_sources_c.push_back("3rdparty/http_parser/http_parser.c");

    std::vector<std::string> multipart_parser_sources_c;
    multipart_parser_sources_c.push_back("3rdparty/multipart_parser/multipart_parser.c");

    std::ofstream outputHeadersHppFile ("webmvcpp_headers_hpp_amalgamation.c", std::ofstream::out);
    outputHeadersHppFile << "const char webmvcpp_headers_hpp_amalgamation[] = { \\" << std::endl;
    codeLineToHex("extern \"C\" {", outputHeadersHppFile);
    outputHeadersHppFile << "0x0D, 0x0A, " << " \\" << std::endl;
    codeLineToHex("#include \"webmvcpp_multipart_parser.h\"", outputHeadersHppFile);
    outputHeadersHppFile << "0x0D, 0x0A, " << " \\" << std::endl;
    codeLineToHex("#include \"webmvcpp_http_parser.h\"", outputHeadersHppFile);
    outputHeadersHppFile << "0x0D, 0x0A, " << " \\" << std::endl;
    codeLineToHex("}", outputHeadersHppFile);
    outputHeadersHppFile << "0x0D, 0x0A, " << " \\" << std::endl;
    for(std::vector<std::string>::iterator it = headers_hpp.begin();it != headers_hpp.end();++it) {
        appendFileToContent((*it).c_str(), outputHeadersHppFile);
    }
    outputHeadersHppFile << "      0x00 };";
    outputHeadersHppFile.close();

    std::ofstream outputSourcesCppFile ("webmvcpp_sources_cpp_amalgamation.c", std::ofstream::out);
    outputSourcesCppFile << "const char webmvcpp_sources_cpp_amalgamation[] = { \\" << std::endl;
    codeLineToHex("#include \"webmvcpp.hpp\"", outputSourcesCppFile);
    outputSourcesCppFile << "0x0D, 0x0A, " << " \\" << std::endl;
    for(std::vector<std::string>::iterator it = sources_cpp.begin();it != sources_cpp.end();++it) {
        appendFileToContent((*it).c_str(), outputSourcesCppFile);
    }
    outputSourcesCppFile << "      0x00 };";
    outputSourcesCppFile.close();

    std::ofstream outputHttpParserHFile ("webmvcpp_http_parser_h.c", std::ofstream::out);
    outputHttpParserHFile << "const char webmvcpp_http_parser_h[] = { \\" << std::endl;
    for(std::vector<std::string>::iterator it = http_parser_headers_h.begin();it != http_parser_headers_h.end();++it) {
        appendFileToContent((*it).c_str(), outputHttpParserHFile);
    }
    outputHttpParserHFile << "      0x00 };";
    outputHttpParserHFile.close();

    std::ofstream outputMultipartParserHFile ("webmvcpp_multipart_parser_h.c", std::ofstream::out);
    outputMultipartParserHFile << "const char webmvcpp_multipart_parser_h[] = { \\" << std::endl;
    for(std::vector<std::string>::iterator it = multipart_parser_headers_h.begin();it != multipart_parser_headers_h.end();++it) {
        appendFileToContent((*it).c_str(), outputMultipartParserHFile);
    }
    outputMultipartParserHFile << "      0x00 };";
    outputMultipartParserHFile.close();

    return 0;
}
