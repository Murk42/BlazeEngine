#include "pch.h"
#include "BlazeEngine/UI/Nodes/Label.h"

namespace Blaze::UI::Nodes
{
	Label::Label()
		: renderDataDirty(false), textSize(0.0f)
	{
		dataMap.SetTypeName("Label");

		finalTransformUpdatedEventDispatcher.AddHandler<&Label::FinalTransformUpdatedEvent>(*this);
		transformFilterEventDispatcher.AddHandler<&Label::TransformFilterEvent>(*this);
	}
	Label::Label(Node& parent, const NodeTransform& transform)
		: Label()
	{
		SetTransform(transform);
		SetParent(&parent);
	}
	Label::Label(Node& parent, const NodeTransform& transform, u8StringView string, const FontAtlas& atlas, ColorRGBAf color, float fontSize)
		: Label(parent, transform)
	{
		BuildText(string, atlas, fontSize);
		SetColor(color);
	}
	Label::~Label()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Label::FinalTransformUpdatedEvent>(*this);
		transformFilterEventDispatcher.RemoveHandler<&Label::TransformFilterEvent>(*this);
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
	void Label::SetColor(ColorRGBAf color)
	{
		renderUnit.color = color;
	}
	void Label::SetBlocksHitTestFlag(bool blocksHitTest)
	{
		this->blocksHitTest = blocksHitTest;
	}
	void Label::PreRender(const RenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderDataDirty)
			return;

		renderDataDirty = false;

		auto transform = GetFinalTransform();
		renderUnit.position = transform.position + Vec2f(0, textSize.y);
		renderUnit.right = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
	}
	RenderUnitBase* Label::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	auto Label::HitTest(Vec2f screenPosition) -> HitStatus
	{
		HitStatus hitStatus = Node::HitTest(screenPosition);
		if (blocksHitTest)
			if (hitStatus != HitStatus::NotHit)
				return HitStatus::HitBlocking;
			else
				return HitStatus::NotHit;
		else
			if (hitStatus != HitStatus::NotHit)
				return HitStatus::HitNotBlocking;
			else
				return HitStatus::NotHit;
	}
	void Label::TransformFilterEvent(const Node::TransformFilterEvent& event)
	{
		event.transform.size = textSize;
	}
	void Label::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderDataDirty = true;
	}

}