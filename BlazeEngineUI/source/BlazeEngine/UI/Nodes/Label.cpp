#include "pch.h"
#include "BlazeEngine/UI/Nodes/Label.h"
#include "BlazeEngine/UI/Core/Screen.h"

namespace Blaze::UI::Nodes
{
	Label::Label()
		: rendererTypeIDChanged(false), blocksHitTest(true), textBuildSize(0.0f), wrapWidth(FLT_MAX)
	{
		dataMap.SetTypeName("Label");

		finalTransformUpdatedEventDispatcher.AddHandler<&Label::FinalTransformUpdatedEvent>(*this);
	}
	Label::Label(Node& parent, const NodeTransform& transform)
		: Label()
	{
		SetTransform(transform);
		SetParent(&parent);
	}
	Label::Label(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String text)
		: Label(parent, transform)
	{
		SetTextStyle(textStyle);
		SetText(text);
	}
	Label::~Label()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Label::FinalTransformUpdatedEvent>(*this);
	}
	void Label::Clear()
	{
		text.Clear();
		textBuildSize = { 0.0f, 0.0f};
		renderUnit.Clear();

		UpdateTransformSize();
	}
	void Label::SetText(u8String text)
	{
		this->text = std::move(text);

		UpdateRenderUnit();
	}
	void Label::SetTextStyle(const TextStyle& newTextStyle)
	{
		bool fontChanged = false;
		bool fontHeightChanged = false;

		if (newTextStyle.fontName != textStyle.fontName)
			fontChanged = true;

		if (newTextStyle.fontHeight != textStyle.fontHeight)
			fontHeightChanged = true;

		textStyle = newTextStyle;

		if (fontChanged || fontHeightChanged)
			UpdateRenderUnit();		

		renderUnit.SetColor(textStyle.color);
	}
	void Label::SetWrapWidth(float newWrapWidth)
	{
		bool wrapWidthChanged = wrapWidth != newWrapWidth;

		wrapWidth = newWrapWidth;

		if (wrapWidthChanged)
			UpdateRenderUnit();
	}
	void Label::SetBlocksHitTestFlag(bool blocksHitTest)
	{
		this->blocksHitTest = blocksHitTest;
	}
	bool Label::PreRender(const RenderContext& renderContext)
	{
		bool returnValue = false;

		returnValue |= rendererTypeIDChanged;
		rendererTypeIDChanged = false;

		return returnValue;
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
	bool Label::GetFontAtlasData(FontManager::FontAtlasData& fontAtlasData) const
	{
		FontManager* fontManager = nullptr;

		if (GetScreen() != nullptr)
			fontManager = GetScreen()->resourceManager.GetResource<FontManager>("fontManager").GetValue();

		if (fontManager == nullptr)
		{
			BLAZE_LOG_WARNING("There is no font manager assigned to the scene");
			return false;
		}

		const auto& finalTransform = GetFinalTransform();

		TextStyle finalTextStyle = textStyle;
		finalTextStyle.fontHeight *= finalTransform.scale;
		
		if (!fontManager->GetFontAtlas(finalTextStyle, fontAtlasData))
		{
			BLAZE_LOG_WARNING("Couldn't find font atlas with font name: \"{}\"", finalTextStyle.fontName);
			return false;
		}

		if (fontAtlasData.atlas == nullptr)
		{
			BLAZE_LOG_WARNING("Font manager returned a nullptr atlas");
			return false;
		}

		if (fontAtlasData.fontFace == nullptr)
		{
			BLAZE_LOG_WARNING("Font manager returned a nullptr font face");
			return false;
		}

		return true;
	}
	void Label::UpdateTransformSize()
	{
		auto transform = GetTransform();
		transform.size = textBuildSize / renderUnit.GetAtlasFontHeight() * textStyle.fontHeight;
		SetTransform(transform);
	}
	void Label::UpdateRenderUnitTransform()
	{
		const auto& finalTransform = GetFinalTransform();
		Vec2f finalTextSize = textBuildSize / renderUnit.GetAtlasFontHeight() * renderUnit.GetTargetFontHeight();
		renderUnit.SetTransform(finalTransform.position + finalTransform.Up() * finalTextSize.y, finalTransform.right);
	}
	void Label::UpdateRenderUnit()
	{
		if (text.Empty() || textStyle.fontHeight == 0 || textStyle.fontName.Empty())
		{
			renderUnit.Clear();
			return;
		}

		FontManager::FontAtlasData atlasData;

		if (!GetFontAtlasData(atlasData))
		{
			renderUnit.Clear();
			return;
		}
		
		const auto& finalTransform = GetFinalTransform();
		renderUnit.SetTargetFontHeight(textStyle.fontHeight * finalTransform.scale);

		rendererTypeIDChanged = atlasData.rendererTypeID != renderUnit.GetRequiredRendererTypeID();
		textBuildSize = renderUnit.BuildWrappedText(text, atlasData, wrapWidth / textStyle.fontHeight * atlasData.atlas->GetRasterFontHeight());

		UpdateTransformSize();
		UpdateRenderUnitTransform();
	}
	void Label::FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		const auto& finalTransform = GetFinalTransform();

		if (finalTransform.scale * textStyle.fontHeight != renderUnit.GetTargetFontHeight())
			UpdateRenderUnit();
		else
			UpdateRenderUnitTransform();
	}
}