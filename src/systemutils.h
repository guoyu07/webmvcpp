#ifndef WEBMVCPP_SYSTEMUTILS_H
#define WEBMVCPP_SYSTEMUTILS_H

namespace webmvcpp
{
	struct systemutils {
		static void sleep(unsigned int msec)
		{
#if defined(_WIN32)
			::Sleep(msec);
#else
			::usleep(msec * 1000);
#endif
		}

		static std::string queryBuildPlatform()
		{
#if defined(_WIN64)
			return "MS Windows 64-bit build";
#elif (_WIN32)
			return "MS Windows 32-bit build";
#elif (__APPLE__)
	#include "TargetConditionals.h"
#if (TARGET_IPHONE_SIMULATOR)
			return "Apple iOS (emulator) build";
#elif (TARGET_OS_IPHONE)
			return "Apple iOS build";
#elif (TARGET_OS_MAC)
			return "Apple OS X build";
#else
			return "Unknown Apple build";
#endif
#elif (__linux)
			return "Linux build";
#elif (__FreeBSD__)
			return "FreeBSD build";
#elif (__unix) // all unices not caught above
			return "Unknown UNIX build";
#elif (__posix)
			return "Unknown POSIX build";
#endif
		}

		static std::string getUserName()
		{
			std::string userName = "unknown";

#if defined(_WIN32)

			WCHAR userNameBuffer[MAX_PATH + 1];
			ULONG userNameBufferLength = sizeof(userNameBuffer) / sizeof(userNameBuffer[0]) - 1;

			GetUserNameW(userNameBuffer, &userNameBufferLength);

			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
			userName = utf8conv.to_bytes(userNameBuffer);
#elif defined (__APPLE__)

			userName = getlogin();

#elif defined (__linux)

			passwd *pw = NULL;
			pw = getpwuid(getuid());
			if (pw)
				userName = pw->pw_name;

#endif 

			return userName;
		}

		static std::string getSystemName()
		{
			std::string osNameValue;

#if defined(_WIN32)

			OSVERSIONINFO osi;
			osi.dwOSVersionInfoSize = sizeof(osi);
			GetVersionEx(&osi);

			std::ostringstream ntStr;
			ntStr << " NT " << osi.dwMajorVersion << '.' << osi.dwMinorVersion;
			osNameValue += ntStr.str();

			osNameValue = "Windows ";

			switch (osi.dwMajorVersion)
			{
				break;
			case 5:
			{
				switch (osi.dwMinorVersion)
				{
				case 0:
					osNameValue += "2000";
					break;
				case 1:
					osNameValue += "XP";
					break;
				case 2:
					osNameValue += "2003";
					break;
				}
			}
			break;
			case 6:
			{
				switch (osi.dwMinorVersion)
				{
				case 0:
					osNameValue += "Vista";
					break;
				case 1:
					osNameValue += "7";
					break;
				case 2:
					osNameValue += "8";
					break;
				}
			}
			break;
			}

			osNameValue += ntStr.str();

#elif defined (__APPLE__)

			char str[256] = {0};
			size_t size = sizeof(str);
			sysctlbyname("kern.osrelease", str, &size, NULL, 0);
			std::istringstream kernelRelease(str);

			int kernelMajorVesion = 0;
			kernelRelease >> kernelMajorVesion;


			switch (kernelMajorVesion)
			{
				break;
			case 5:
				osNameValue += "Puma";
				break;
			case 6:
				osNameValue += "Jaguar";
				break;
			case 7:
				osNameValue += "Punther";
				break;
			case 8:
				osNameValue += "Tiger";
				break;
			case 9:
				osNameValue += "Leopard";
				break;
			case 10:
				osNameValue += "Snow Leopard";
				break;
			case 11:
				osNameValue += "Lion";
				break;
			case 12:
				osNameValue += "Mountain Lion";
				break;
			case 13:
				osNameValue += "Mavericks";
				break;
			default:
				osNameValue += "Unknown";
			}

#elif defined (ANDROID)

			osNameValue = "Android ";

			std::ifstream ctrlPageFile("/system/build.prop");
			if (ctrlPageFile.is_open())
			{
				std::string oneProperty;
				while (std::getline(vFile, oneProperty))
				{
					std::vector<std::string> splittedLine = split_string(oneLine, '=');
					if (splittedLine.size() == 2 && splittedLine[0] == "ro.build.version.release")
					{
						osNameValue += splittedLine[1];
						break;
					}
				}
			}

#elif defined (__linux)

			std::ifstream vFile("/proc/version");
			std::getline(vFile, osNameValue);

#else

			osNameValue = "Unknown os";

#endif

			return osNameValue;
		}

		static std::string getHostName()
		{
			char hostName[32];
			gethostname(hostName, sizeof hostName);

			return hostName;
		}

		unsigned int getMemorySize()
		{
#if defined (_WIN32)

			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			return (unsigned int)(status.ullTotalPhys / (1024 * 1024));

#elif (__linux)

			long long memsize = (long long)sysconf(_SC_PHYS_PAGES) * sysconf(_SC_PAGESIZE);
			return memsize / (1024 * 1024);

#elif defined (__APPLE__) || defined (__FreeBSD__)

			int mib[2] = { CTL_HW, HW_MEMSIZE };
			u_int namelen = sizeof(mib) / sizeof(mib[0]);
			uint64_t memsize = 0;
			size_t len = sizeof(memsize);

			if (sysctl(mib, namelen, &memsize, &len, NULL, 0) < 0)
				return 0;
			else
				return memsize / (1024 * 1024);

#endif
			return 0;
		}

		static std::string getApplicationPath(const char** argv)
		{
			std::string appPath;
#if defined (_WIN32)
			char pathBuffer[MAX_PATH + 2];
			DWORD l = GetModuleFileNameA(0, pathBuffer, MAX_PATH + 1);
			pathBuffer[l] = 0;
			appPath = pathBuffer;
#elif (__linux)
			appPath = getauxval(AT_EXECFN);
#elif (__APPLE__)
			char pathBuffer[PATH_MAX] = { 0 };
			uint32_t size = sizeof(pathBuffer);
			_NSGetExecutablePath(pathBuffer, &size);
			pathBuffer[size] = 0;
			appPath = pathBuffer;
#endif
			return appPath;
		}
	};
}


#endif // WEBMVCPP_SYSTEMUTILS_H
