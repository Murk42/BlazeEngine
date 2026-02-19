#include "pch.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Nodes/TextInput.h"
#include "BlazeEngine/UI/Text/FontManager.h"

namespace Blaze::UI::Nodes
{
	TextInput::TextInput()
	{
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform)
		: InputNode(parent, transform)
	{
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText, u8String text)
		: TextInput(parent, transform)
	{
		SetTextStyle(textStyle);
		SetEmptyText(emptyText);
		SetText(text);
	}
	TextInput::~TextInput()
	{
	}
	void TextInput::SetText(u8String newText)
	{
		if (newText == text)
			return;

		text = std::move(newText);

		UpdateRenderUnits();
	}
	void TextInput::SetEmptyText(u8String newEmptyText)
	{
		if (newEmptyText == emptyText)
			return;

		emptyText = std::move(newEmptyText);

		if (text.Empty())
			UpdateRenderUnits();
	}
	void TextInput::SetTextStyle(const TextStyle& newTextStyle)
	{
		bool fontChanged = false;
		bool fontHeightChanged = false;

		if (newTextStyle.fontName != textStyle.fontName)
			fontChanged = true;

		if (newTextStyle.fontHeight != textStyle.fontHeight)
			fontHeightChanged = true;

		textStyle = newTextStyle;

		if (fontChanged || fontHeightChanged)
			UpdateRenderUnits();

		textRenderUnit.SetColor(textStyle.color);
	}
	void TextInput::SetMultiline(bool newMultiline)
	{
		if (multiline == newMultiline)
			return;

		multiline = newMultiline;
		UpdateRenderUnits();
	}
	bool TextInput::PreRender(const RenderContext& renderContext)
	{
		bool returnValue = false;

		returnValue |= rendererTypeIDChanged;
		rendererTypeIDChanged = false;

		return returnValue;
	}
	RenderUnitBase* TextInput::GetRenderUnit(uintMem index)
	{
		switch (index)
		{
		case 0:
			return &textRenderUnit;
		case 1:
			return &cursorRenderUnit;
		default:
			return nullptr;
		}
	}
	void TextInput::TransformUpdated(const TransformUpdatedEvent& event)
	{
		auto& transform = GetTransform();

		if (transform.size.x != textWrapWidth)
		{
			textWrapWidth = transform.size.x;

			if (multiline)
				UpdateRenderUnits();
		}
	}
	bool TextInput::GetFontAtlasData(FontManager::FontAtlasData& fontAtlasData) const
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
	void TextInput::UpdateTextRenderUnitTransform()
	{
		const auto& finalTransform = GetFinalTransform();
		Vec2f finalTextSize = textBuildSize / textRenderUnit.GetAtlasFontHeight() * textRenderUnit.GetTargetFontHeight();
		textRenderUnit.SetTransform(finalTransform.position + finalTransform.Up() * finalTextSize.y, finalTransform.right);
	}
	void TextInput::UpdateRenderUnits()
	{
		if (text.Empty() || textStyle.fontHeight == 0 || textStyle.fontName.Empty())
		{
			textRenderUnit.Clear();
			return;
		}

		FontManager::FontAtlasData atlasData;

		if (!GetFontAtlasData(atlasData))
		{
			textRenderUnit.Clear();
			return;
		}

		const auto& finalTransform = GetFinalTransform();
		textRenderUnit.SetTargetFontHeight(textStyle.fontHeight * finalTransform.scale);

		rendererTypeIDChanged = atlasData.rendererTypeID != textRenderUnit.GetRequiredRendererTypeID();
		if (multiline)
			textBuildSize = textRenderUnit.BuildText(text, atlasData);
		else
			textBuildSize = textRenderUnit.BuildWrappedText(text, atlasData, textWrapWidth / textStyle.fontHeight * atlasData.atlas->GetRasterFontHeight());

		UpdateTextRenderUnitTransform();
	}
}