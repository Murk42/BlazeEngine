#pragma once
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze
{
	struct CharacterLayoutData
	{
		Vec2f pos;
		Vec2f size;
		UnicodeChar character;
	};
	struct TextLineLayoutData
	{
		Array<CharacterLayoutData> characters;
		float width;
	};	

	class BLAZE_API TextLayouterBase
	{
	public:
		virtual ~TextLayouterBase() { }
		
		inline ArrayView<TextLineLayoutData> GetLines() const { return lines; }

		virtual void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) = 0;
	protected:
		TextLayouterBase() = default;

		Array<TextLineLayoutData> lines;
	};

	class BLAZE_API SingleLineTextLayouter : public TextLayouterBase
	{
	public:
		SingleLineTextLayouter();
		SingleLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics);

		void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) override;
	};

	class BLAZE_API MultiLineTextLayouter : public TextLayouterBase
	{
	public:
		MultiLineTextLayouter();
		MultiLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics);

		void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) override;
	};	

	class BLAZE_API WrappedLineTextLayouter : public TextLayouterBase
	{
		//Measured in pixels
		float wrapWidth;
	public:
		WrappedLineTextLayouter();
		WrappedLineTextLayouter(StringViewUTF8 text, const FontMetrics& fontMetrics, float wrapWidth = 0);

		//Set wrap width in pixel measures, if wrapWidth is 0 no wrapping is applied
		void SetWrapWidth(float wrapWidth);
		void SetText(StringViewUTF8 text, const FontMetrics& fontMetrics) override;
	};
}