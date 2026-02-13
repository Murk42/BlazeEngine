#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Text/Core/TextStyle.h"

namespace Blaze
{
	struct TextStyleSpan
	{
		ResourceRef<TextStyle> style;

		uintMem startIndex;
		uintMem endIndex;

		bool operator==(const TextStyleSpan& other) const = default;
		bool operator!=(const TextStyleSpan& other) const = default;
	};
}