#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/Core/Event/EventDispatcher.h"
#include "BlazeEngine/Core/Resource/ResourceRef.h"
#include "BlazeEngine/UI/Text/FontFace.h"

namespace Blaze::UI
{
	struct TextStyle
	{	
		String fontName = "";

		float fontHeight = 20.0f;
		ColorRGBAf color = 0xFDFDFDFF;

		bool underline : 1 = false;
		bool strikethrough : 1 = false;

		float baselineOffset = 0.0f; //for super-script for example
	};
}