#pragma once

#pragma warning (disable : 4996 )

#ifdef BLAZE_EXPORTS
#define BLAZE_API __declspec(dllexport)
#else
#define BLAZE_API __declspec(dllimport)
#endif

namespace Blaze
{
	unsigned BLAZE_API GetMajorVersion();
	unsigned BLAZE_API GetMinorVersion();
}