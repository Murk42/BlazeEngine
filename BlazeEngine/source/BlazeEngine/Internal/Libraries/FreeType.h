#pragma once
#include "BlazeEngineCore/Utilities/Timing.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

namespace Blaze
{
	FT_Library GetFreeTypeLibrary();

	TimingResult InitializeFreeType();
	void TerminateFreeType();

	FT_Face OpenFaceFromStream(ReadStream& source, int index);
}