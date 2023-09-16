#pragma once
#include "CustomRenderBackend.h"

struct TextRenderData
{
	String text;
	FontResolution* fontResolution;
	Array<ColorRGBAf> charactersColor;
	Vec2f size;
};

struct TextRenderCache;

class TextRenderer : public CustomRenderBackend<TextRenderer>
{
public:
	virtual ~TextRenderer() = 0;

	Handle<TextRenderCache> GenerateRenderCache(const TextRenderData& renderData);

	virtual void Render(const Graphics::Transform2D& transform, const TextRenderData& renderData) = 0;
	virtual void Render(const Graphics::Transform2D& transform, const TextRenderCache& renderCache) = 0;
};