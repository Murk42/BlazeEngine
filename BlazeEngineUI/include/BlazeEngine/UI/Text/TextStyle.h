#pragma once
#include "BlazeEngine/Core/Common/Color.h"

namespace Blaze::UI
{
	class FontManager;
}

namespace Blaze::UI::Nodes
{
	struct TextStyle
	{
		FontManager* fontManager = nullptr;

		String fontName = "";

		float fontHeight = 20.0f;
		ColorRGBAf color = 0xFDFDFDFF;

		bool underline : 1 = false;
		bool strikethrough : 1 = false;

		float baselineOffset = 0.0f; //for super-script for example

		TextStyle Scale(float scale) const;

		bool operator==(const TextStyle&) const = default;
		bool operator!=(const TextStyle&) const = default;
	};
}