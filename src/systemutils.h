#ifndef WEBMVCPP_SYSTEMUTILS_H
#define WEBMVCPP_SYSTEMUTILS_H

namespace webmvcpp
{
    void sleep(unsigned int msec);

	std::string queryBuildPlatform();

	std::string getUserName();

	std::string getSystemName();

    std::string getHostName();

	unsigned int getMemorySize();
}


#endif // WEBMVCPP_SYSTEMUTILS_H
