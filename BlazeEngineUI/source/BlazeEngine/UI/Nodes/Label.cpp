#include "pch.h"
#include "BlazeEngine/UI/Nodes/Label.h"
#include <BlazeEngine/UI/Text/WrappedLineTextShaper.h>
#include <cmath>

namespace Blaze::UI::Nodes
{
	Label::Label()
		: rebuildGlyphs(false), blocksHitTest(true), updateRenderTopology(false), wrapWidth(FLT_MAX), textFinalSize({ 0, 0 })
	{
		dataMap.SetTypeName("Label");		
	}
	Label::Label(Node& parent, const NodeTransform& transform)
		: Label()
	{		
		SetParent(&parent);
		SetTransform(transform);
	}
	Label::Label(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String text)
		: Label(parent, transform)
	{
		SetTextStyle(textStyle);
		SetText(text);
	}
	Label::~Label()
	{
	}
	void Label::Clear()
	{
		renderUnit.Clear();
		MarkTransformDirty();
	}
	void Label::SetText(u8String newText)
	{
		if (text == newText)
			return;

		this->text = std::move(newText);
		rebuildGlyphs = true;
		MarkTransformDirty();
	}
	void Label::SetTextStyle(const TextStyle& newTextStyle)
	{		
		if (textStyle == newTextStyle)
			return;

		TextStyle oldTextStyle = std::move(textStyle);
		textStyle = newTextStyle;

		if (oldTextStyle.color != newTextStyle.color)
			renderUnit.SetColor(textStyle.color);
		
		if (oldTextStyle.fontHeight != newTextStyle.fontHeight || oldTextStyle.fontName != newTextStyle.fontName)
		{
			rebuildGlyphs = true;
			MarkTransformDirty();
		}
	}
	void Label::SetWrapWidth(float newWrapWidth)
	{
		if (wrapWidth == newWrapWidth)
			return;

		wrapWidth = newWrapWidth;
		rebuildGlyphs = true;
		MarkTransformDirty();
	}
	void Label::SetBlocksHitTestFlag(bool blocksHitTest)
	{
		this->blocksHitTest = blocksHitTest;
	}
	bool Label::PreRender(const Graphics::RenderContext& renderContext)
	{
		bool oldUpdateRenderTopology = updateRenderTopology;
		updateRenderTopology = false;
		return oldUpdateRenderTopology;
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
	void Label::UpdateTransform()
	{
		if (rebuildGlyphs)
		{
			RebuildGlyphs();
			rebuildGlyphs = false;
		}

		NodeTransform transform = GetTransform();
		transform.size = textFinalSize / GetFinalScale();
		SetTransform(transform);
	}
	void Label::FinalScaleUpdated(const FinalScaleUpdatedEvent&)
	{
		rebuildGlyphs = true;
		MarkTransformDirty();
	}
	void Label::RebuildGlyphs()
	{
		if (text.Empty() || textStyle.fontHeight == 0 || textStyle.fontName.Empty() || textStyle.fontManager == nullptr)
		{
			renderUnit.Clear();

			textFinalSize = { 0, 0 };
			return;
		}

		uint32 finalFontHeight = static_cast<uint32>(std::round(textStyle.fontHeight * GetFinalScale()));
		float finalWrapWidth = wrapWidth * GetFinalScale();

		FontManager::FontAtlasData atlasData;		
		if (!textStyle.fontManager->GetFontAtlas(textStyle.fontName, finalFontHeight , atlasData))
		{
			BLAZE_LOG_WARNING("Couldn't find font atlas with font name: \"{}\"", textStyle.fontName);
			renderUnit.Clear();
			return;
		}				

		updateRenderTopology = atlasData.rendererTypeID != renderUnit.GetRequiredRendererTypeID();
		
		DefaultTextSeparationData textSeparationData{ text };

		Text::WrappedLineTextShaper textShaper{ text, *atlasData.fontFace, finalFontHeight, textSeparationData, finalWrapWidth };
		auto shapedLines = textShaper.Shape(text);
				
		Array<StaticTextRenderUnit::GlyphRenderData> glyphs = renderUnit.GenerateGlyphRenderData(shapedLines, *atlasData.fontFace, *atlasData.atlas, finalFontHeight, &textFinalSize);

		renderUnit.SetGlyphs(std::move(glyphs), atlasData.rendererTypeID, atlasData.atlas);
	}
}