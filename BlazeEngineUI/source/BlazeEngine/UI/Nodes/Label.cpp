#include "pch.h"
#include "BlazeEngine/UI/Nodes/Label.h"

namespace Blaze::UI::Nodes
{
	Label::Label()
		: renderDataDirty(false), textSize(0.0f)
	{
		dataMap.SetTypeName("Label");

		finalTransformUpdatedEventDispatcher.AddHandler<&Label::FinalTransformUpdatedEvent>(*this);
		transformUpdatedEventDispatcher.AddHandler<&Label::TransformUpdatedEvent>(*this);
	}
	Label::Label(Node& parent, const NodeTransform& transform, u8StringView string, const FontAtlas& atlas, float fontSize)
		: Label()
	{
		SetParent(&parent);
		SetTransform(transform);
		BuildText(string, atlas, fontSize);
	}
	Label::~Label()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Label::FinalTransformUpdatedEvent>(*this);
		transformUpdatedEventDispatcher.RemoveHandler<&Label::TransformUpdatedEvent>(*this);
	}
	void Label::ClearText()
	{
		textSize = Vec2f();
		renderUnit.Clear();
		renderDataDirty = true;
		MarkTransformDirty();
	}
	void Label::BuildText(u8StringView string, const FontAtlas& atlas, float fontSize)
	{
		if (fontSize == 0.0f)
			fontSize = atlas.GetFontPixelHeight();

		textSize = renderUnit.BuildText(string, fontSize, atlas);
		renderDataDirty = true;
		MarkTransformDirty();
	}
	void Label::BuildWrappedText(u8StringView string, const FontAtlas& atlas, float wrapWidth, float fontSize)
	{
		if (fontSize == 0.0f)
			fontSize = atlas.GetFontPixelHeight();

		textSize = renderUnit.BuildWrappedText(string, fontSize, atlas, wrapWidth);
		renderDataDirty = true;
		MarkTransformDirty();
	}
	void Label::PreRender(const UIRenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderDataDirty)
			return;

		renderDataDirty = false;

		auto transform = GetFinalTransform();
		renderUnit.position = transform.position + Vec2f(0, textSize.y);
		renderUnit.right = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
	}
	UIRenderUnitBase* Label::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	void Label::TransformUpdatedEvent(const Node::TransformUpdatedEvent& event)
	{
		auto transform = GetTransform();

		if (transform.size != textSize)
		{
			transform.size = textSize;
			SetTransform(transform);
		}
	}
	void Label::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderDataDirty = true;
	}

}