#pragma once
#include "BlazeEngine/Core/Time/TimingTree.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>

namespace Blaze
{
	class ReadStream;

	FT_Library FT_GetLibrary();

	bool InitializeFreeType(TimingTree& timingTree);
	void TerminateFreeType();

	FT_Face FT_OpenFaceFromStream(ReadStream& source, int index);
}