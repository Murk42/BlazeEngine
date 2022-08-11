#include "source/BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngine/Utilities/Time.h"

#include "freetype/freetype.h"

namespace Blaze
{
	static FT_Library freeTypeLibrary;

	FT_Library GetFreeTypeLibrary()
	{
		return freeTypeLibrary;
	}

	Startup::BlazeLibrariesInitInfo::FreeTypeInitInfo InitializeFreeType()
	{
		Startup::BlazeLibrariesInitInfo::FreeTypeInitInfo initInfo;
		TimePoint startTimePoint = TimePoint::GetWorldTime();

		if (FT_Init_FreeType(&freeTypeLibrary) != 0)
			throw String("Failed to initialize the FreeType libary");

		initInfo.initTime = TimePoint::GetWorldTime() - startTimePoint;
		return initInfo;
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(freeTypeLibrary);
	}
}