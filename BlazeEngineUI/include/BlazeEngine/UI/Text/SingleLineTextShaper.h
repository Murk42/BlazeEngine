#pragma once
#include <BlazeEngine/UI/Text/TextShaper.h>
#include "BlazeEngine/UI/Text/FontFace.h"

namespace Blaze::UI::Text
{
	class SingleLineTextShaper : public TextShaper
	{
	public:
		SingleLineTextShaper(u8StringView textContext, const FontFace& fontFace, uint32 fontHeight);
		~SingleLineTextShaper() override;

		virtual Array<ShapedString> Shape(u8StringView text) override;
		ShapedString ShapeSingleLine(u8StringView text);

		inline u8StringView GetTextContext() const { return textContext; }
		inline uint32 GetFontHeight() const { return fontHeight; }
	private:
		const u8StringView textContext;
		void* fontHandle;
		const uint32 fontHeight;
	};
}