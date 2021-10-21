#pragma once
#include "BlazeEngine/BlazeEngine.h"

#include "BlazeEngine/Graphics/UI/Elements/Element.h"
#include "BlazeEngine/DataStructures/String.h"
#include "BlazeEngine/DataStructures/Color.h"

namespace Blaze
{
	class Font;
}

namespace Blaze::UI
{
	class TextRenderer;
	struct TextVertex;

	class BLAZE_API Text : public Element
	{
		TextRenderer* renderer;

		TextVertex* vertices;
		uint vertexCount;

		String string;

		Vec2f size;
		float scale;

		ColorRGBA color;

		Font* font;
	public:
		bool clip;
		Recti clipRect;

		Text();
		~Text();

		void SetColor(ColorRGBA color);
		void SetScale(float scale);
		void SetString(StringView string);
		void SetFont(Font* font);

		inline Font* GetFont() const { return font; }
		inline ColorRGBA GetColor() const { return color; }
		inline uint GetScale() const { return scale; };
		inline String GetString() const { return string; }

		friend class TextRenderer;
	};
}