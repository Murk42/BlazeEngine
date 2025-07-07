#pragma once
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnits/StaticTextRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze
{
	class TextLayouterBase;
}

namespace Blaze::UI::Nodes
{
	class Text : 
		public Node,
		public Graphics::RenderObject
	{
	public:						
		Text();
		~Text();

		void SetText(StringViewUTF8 text, ArrayView<TextStyleSpan> styleSpans, float maxLineLength, ResourceRef<TextStyle> defaultTextStyle, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases);

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		Graphics::StaticTextRenderUnit renderUnit;

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}