#include "pch.h"
#include "BlazeEngine/Internal/Libraries/FreeType.h"

#include "freetype/freetype.h"
#include "sail-c++/sail-c++.h"

namespace Blaze
{
	static FT_Library freeTypeLibrary;

	FT_Library GetFreeTypeLibrary()
	{
		return freeTypeLibrary;
	}

	static StringView GetFTError(FT_Error error)
	{
		const char* ptr = FT_Error_String(error);
		uintMem len = strlen(ptr);
		return StringView(ptr, len);
	}

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

	Timing InitializeCoreFreeType()
	{
		Timing timing{ "FreeType initialization" };	

		if (auto error = FT_Init_FreeType(&freeTypeLibrary))
			Debug::Logger::LogFatal("Blaze Engine", "Failed to initialize the FreeType libary. FT_Error_String returned \"" + GetFTError(error) + "\"");
		
		Debug::Logger::LogInfo("Blaze Engine", "Successfully initialized FreeType " STRINGIZE(FREETYPE_MAJOR) "." STRINGIZE(FREETYPE_MINOR) "." STRINGIZE(FREETYPE_PATCH));

		return timing;
	}

	TimingResult InitializeFreeType()
	{
		Timing timing{ "FreeType" };
		
		timing.AddNode(InitializeCoreFreeType());

		return timing;
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(freeTypeLibrary);
	}
}