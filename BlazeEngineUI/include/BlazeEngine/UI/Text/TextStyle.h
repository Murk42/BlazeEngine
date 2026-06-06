#pragma once
#include "BlazeEngine/Core/Common/Color.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze::UI::Nodes
{
	struct TextStyle
	{
		FontManager* fontManager = nullptr;

		String fontName = "";

		uint32 fontHeight = 20;
		ColorRGBAf color = 0xFDFDFDFF_rgba;

		bool underline : 1 = false;
		bool strikethrough : 1 = false;

		float baselineOffset = 0.0f; //for super-script for example

		TextStyle Scale(float scale) const;

		FontManager::FontData GetFontData() const;

		bool operator==(const TextStyle&) const = default;
		bool operator!=(const TextStyle&) const = default;
	};
}