#pragma once
#include "BlazeEngine/UI/Graphics/Renderers/TextRendererBase.h"
#include "BlazeEngine/UI/Text/FontGlyphRasterizer.h"

namespace Blaze::Graphics
{
	class BLAZE_API AntialiasedTextRenderer : public TextRendererBase
	{
	public:
		const UI::FontGlyphRasterizer& GetFontGlyphRasterizer() const { return glyphRasterizer; }

		RendererTypeID GetTypeID() const override final { return RendererBase::GetTypeIDFor<AntialiasedTextRenderer>(); }
	private:
		UI::FontGlyphRasterizers::AntialiasedFontGlyphRasterizer glyphRasterizer;
	};
}