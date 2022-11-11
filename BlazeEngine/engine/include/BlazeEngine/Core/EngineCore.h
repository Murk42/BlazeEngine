#pragma once

#pragma warning (disable : 4996 )

#ifdef FINAL_BUILD
#define BLAZE_API
#else
#define BLAZE_API __declspec(dllexport)
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