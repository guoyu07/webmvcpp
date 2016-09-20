#include <algorithm>
#include <functional> 

#include <memory>
#include <string>
#include <list>
#include <deque>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <mutex>
#include <thread>
#include <locale>
#include <atomic>

#include <cctype>
#include <locale>
#include <iomanip>

#include <stdio.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string.h>

#ifdef _WIN32
#include <codecvt>
#include <winsock.h>
#else
#include <dlfcn.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef __linux
#include <unistd.h>
#include <pwd.h>
#include <sys/auxv.h>
#endif 

#ifdef __APPLE__
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <CoreServices/CoreServices.h>
#include <mach-o/dyld.h> 
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
