#pragma once
#include <BlazeEngine/UI/Text/TextShaper.h>
#include "BlazeEngine/UI/Text/TextStyle.h"

namespace Blaze::UI::Text
{
	class SingleLineTextShaper : public TextShaper
	{
	public:
		SingleLineTextShaper(u8StringView textContext, const Nodes::TextStyle& textStyle);
		~SingleLineTextShaper() override;

		virtual Array<ShapedString> Shape(u8StringView text) override;
		ShapedString ShapeSingleLine(u8StringView text);

		inline u8StringView GetTextContext() const { return textContext; }
		inline const Nodes::TextStyle& GetTextStyle() const { return textStyle; }
	private:
		const u8StringView textContext;
		const Nodes::TextStyle& textStyle;
		void* fontHandle;
	};
}