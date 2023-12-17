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

	Timing InitializeCoreFreeType()
	{
		Timing timing{ "FreeType initialization" };
		
		sail_init(); 

		if (FT_Init_FreeType(&freeTypeLibrary) != 0)
			throw String("Failed to initialize the FreeType libary");

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