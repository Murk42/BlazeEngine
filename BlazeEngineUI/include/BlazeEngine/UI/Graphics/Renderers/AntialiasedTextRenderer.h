#pragma once
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"

namespace Blaze::Graphics
{
	class AntialiasedTextRenderer : public TextRendererBase
	{
	public:
		const UI::FontGlyphRasterizer& GetFontGlyphRasterizer() const { return glyphRasterizer; }

		RendererTypeID GetTypeID() const { return RendererBase::GetTypeID<AntialiasedTextRenderer>(); }
	private:
		UI::FontGlyphRasterizers::AntialiasedFontGlyphRasterizer glyphRasterizer;
	};
}