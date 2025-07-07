#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/Text.h"

namespace Blaze::UI::Nodes
{
	Text::Text()
	{		
		dataMap.SetTypeName("Text");		

		finalTransformUpdatedEventDispatcher.AddHandler<&Text::FinalTransformUpdatedEvent>(*this);
	}
	Text::~Text()
	{	
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Text::FinalTransformUpdatedEvent>(*this);
	}
	void Text::SetText(StringViewUTF8 text, ArrayView<TextStyleSpan> styleSpans, float maxLineLength, ResourceRef<TextStyle> defaultTextStyle, const Map<FontAtlasIdentifier, const FontAtlas&>& atlases)
	{
		Rectf rect = renderUnit.SetText(text, styleSpans, maxLineLength, defaultTextStyle, atlases);

		auto transform = GetTransform();
		transform.size = rect.size;
		SetTransform(transform);
	}	
	Graphics::RenderUnit* Text::GetRenderUnit(uint index)
	{
		if (index == 0)					
			return &renderUnit;

		return nullptr;
	}
	void Text::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		auto transform = GetFinalTransform();		
		renderUnit.SetTransform(transform.position + Vec2f(0, transform.size.y), Vec2f(Math::Cos(transform.rotation), Math::Sin(transform.rotation)));
	}

}