#include "source/BlazeEngine/Internal/Libraries/FreeType.h"
#include "BlazeEngine/DataStructures/String.h"

#include "freetype/freetype.h"

namespace Blaze
{
	static FT_Library freeTypeLibrary;

	FT_Library GetFreeTypeLibrary()
	{
		return freeTypeLibrary;
	}

	void InitializeFreeType()
	{
		if (FT_Init_FreeType(&freeTypeLibrary) != 0)
			throw String("Failed to initialize the FreeType libary");
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(freeTypeLibrary);
	}
}