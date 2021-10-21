#include "BlazeEngine/Graphics/UI/Elements/Text.h"
#include "BlazeEngine/Graphics/UI/Elements/TextRenderer.h"
#include "BlazeEngine/Resources/Font/Font.h"

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

		for (size_t i = 0; i < vertexCount; ++i)
			vertices[i].color = color;
	}
	void Text::SetScale(float scale)
	{
		this->scale = scale;
		transform.size = size * scale;
	}

	void Text::SetString(StringView string)
	{
		if (font != nullptr)
		{
			Font::CharacterVertex* buffer = font->GenerateVertices(string, vertexCount, size);

			delete[] vertices;
			vertices = new TextVertex[vertexCount];

			for (size_t i = 0; i < vertexCount; ++i)
			{
				vertices[i].p1 = buffer[i].p1;
				vertices[i].p2 = buffer[i].p2;
				vertices[i].uv1 = buffer[i].uv1;
				vertices[i].uv2 = buffer[i].uv2;
				vertices[i].color = color;
			}

			transform.size = size * scale;
			UpdateTransform();
		}
		this->string = string;
	}

	void Text::SetFont(Font* font)
	{
		this->font = font;
		if (string.Size() != 0)
		{
			Font::CharacterVertex* buffer = font->GenerateVertices(string, vertexCount, size);

			delete[] vertices;
			vertices = new TextVertex[vertexCount];

			for (size_t i = 0; i < vertexCount; ++i)
			{
				vertices[i].p1 = buffer[i].p1;
				vertices[i].p2 = buffer[i].p2;
				vertices[i].uv1 = buffer[i].uv1;
				vertices[i].uv2 = buffer[i].uv2;
				vertices[i].color = color;
			}

			transform.size = size * scale;
			UpdateTransform();
		}
	}
}