#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_EXPORTS
#define BLAZE_API __declspec(dllexport)
#else
#define BLAZE_API __declspec(dllimport)
#endif

#ifdef _WIN32 
#define BLAZE_PLATFORM_WINDOWS
#elif __linux__
#define BLAZE_PLATFORM_LINUX
#elif __APPLE__
#define BLAZE_PLATFORM_APPLE
#endif

#define BLAZE_OK 1
#define BLAZE_YES 1
#define BLAZE_NO 0
#define BLAZE_ERROR -1

namespace Blaze
{
	unsigned BLAZE_API GetMajorVersion();
	unsigned BLAZE_API GetMinorVersion();
}