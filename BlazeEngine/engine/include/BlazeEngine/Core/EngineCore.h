#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_EXPORTS
#define BLAZE_API __declspec(dllexport)
#else
#define BLAZE_API __declspec(dllimport)
#endif

#define BLAZE_OK 1
#define BLAZE_ERROR 0

namespace Blaze
{
	unsigned BLAZE_API GetMajorVersion();
	unsigned BLAZE_API GetMinorVersion();
}