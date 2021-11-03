#include "BlazeEngine/Graphics/UI/Elements/Text.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze::UI
{
	Text::Text()
		: scale(1), vertices(nullptr), font(nullptr), vertexCount(0), clip(false), clipRect(0, 0, 0, 0)
	{		
	}

	Text::~Text()
	{
		delete[] vertices;
	}

	void Text::SetColor(ColorRGBA color)
	{
		this->color = color;		
		dirty = true;
	}
	void Text::SetScale(float scale)
	{
		this->scale = scale;
		transform.size = size * scale;
		UpdateTransform();
	}

	void Text::SetString(StringView string)
	{		
		this->string = string;
		dirty = true;
	}

	void Text::SetFont(Font* font)
	{
		this->font = font;
		dirty = true;
	}
}