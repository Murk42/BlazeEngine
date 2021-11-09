#include "source/BlazeEngine/Internal/Libraries/FreeType.h"
#include "source/BlazeEngine/Internal/Engine.h"
#include "BlazeEngine/DataStructures/String.h"

#include "freetype/freetype.h"

namespace Blaze
{
	void InitializeFreeType()
	{
		if (FT_Init_FreeType(&engine->FreeType.freeTypeLibrary) != 0)
			throw String("Failed to initialize the FreeType libary");
	}
	void TerminateFreeType()
	{
		FT_Done_FreeType(engine->FreeType.freeTypeLibrary);
	}
}