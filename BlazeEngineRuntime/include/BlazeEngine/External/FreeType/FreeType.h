#pragma once
#include "BlazeEngine/Core/Time/Timing.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

namespace Blaze
{
	class ReadStream;

	FT_Library FT_GetLibrary();

	TimingResult InitializeFreeType();
	void TerminateFreeType();

	FT_Face FT_OpenFaceFromStream(ReadStream& source, int index);
}