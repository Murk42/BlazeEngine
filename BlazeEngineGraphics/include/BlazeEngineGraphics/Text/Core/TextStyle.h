#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineCore/DataStructures/String.h"
#include "BlazeEngineCore/Resource/ResourceRef.h"
#include "BlazeEngine/Resources/Font/FontFace.h"

namespace Blaze
{	
	class FontFace;	

	struct TextStyle
	{
		ResourceRef<FontFace> fontFace;

		float fontSize;
		ColorRGBA color;
		
		bool underline : 1;
		bool strikeThrough : 1;

		float baselineOffset; //for super-script for example
	};

	struct TextStyleSpan
	{
		ResourceRef<TextStyle> style;

		uintMem startIndex;
		uintMem endIndex;

		bool operator==(const TextStyleSpan& other) const
		{
			return style == other.style && startIndex == other.startIndex && endIndex == other.endIndex;
		}
		bool operator!=(const TextStyleSpan& other) const
		{
			return style != other.style || startIndex != other.startIndex || endIndex != other.endIndex;
		}
	};
}