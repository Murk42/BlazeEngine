#include "pch.h"
#include "BlazeEngine/Core/String/StringCharIterator.h"
#include "BlazeEngine/UI/Nodes/TextInput.h"
#include "BlazeEngine/Runtime/IO/Input.h"
#include <BlazeEngine/UI/Text/WrappedLineTextShaper.h>
#include "BlazeEngine/UI/Text/TextSeparations.h"
#include <cctype>

namespace Blaze::UI::Nodes
{
	class TextInputSeparationData : public DefaultTextSeparationData
	{
	public:
		TextInputSeparationData(u8StringView string)
		{
			SetText(string);
		}

		bool IsAdditionalWordCharacter(UnicodeChar character) override
		{
			return true;
		}
	};

	class TextInputTextShaper : public Text::WrappedLineTextShaper
	{
	public:
		TextInputTextShaper(u8StringView textContext, const FontFace& fontFace, uint32 fontHeight, const TextSeparationDataBase& textSeparationData, float maxWidth)
			: WrappedLineTextShaper(textContext, fontFace, fontHeight, textSeparationData, maxWidth)
		{

		}
	private:
		bool ShouldIncludeAtEnd(UnicodeChar character) override { return false; }
	};

	TextInput::TextInput()
		: multiline(false), rebuildGlyphs(false), updatetCursorRenderUnit(false), updateRenderTopology(false), textWrapWidth(FLT_MAX), emptyTextColor(0x505050ff)
	{
		cursorRenderUnit.SetHideFlag(!IsSelected());
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform)
		: TextInput()
	{
		SetTransform(transform);
		SetParent(&parent);
	}
	TextInput::TextInput(Node& parent, const NodeTransform& transform, const TextStyle& textStyle, u8String emptyText, u8String text)
		: TextInput(parent, transform)
	{
		SetTextStyle(textStyle);
		SetEmptyText(std::move(emptyText));
		SetText(std::move(text));
	}
	TextInput::~TextInput()
	{
	}
	bool TextInput::SetText(u8String newText)
	{
		if (multiline)
			newText.Replace(u8"\n", { });

		if (FilterEnteredText(newText))
		{
			enteredText = std::move(newText);
			inputControl.SetText(enteredText);
			rebuildGlyphs = true;

			return true;
		}
		else
			return false;
	}
	void TextInput::SetEmptyText(u8String newEmptyText)
	{
		if (multiline)
			newEmptyText.Replace(u8"\n", { });

		if (emptyText == newEmptyText)
			return;

		emptyText = std::move(newEmptyText);

		if (inputControl.GetText().Empty())
			rebuildGlyphs = true;
	}
	void TextInput::SetTextStyle(const TextStyle& newTextStyle)
	{
		if (textStyle == newTextStyle)
			return;

		TextStyle oldTextStyle = std::move(textStyle);
		textStyle = newTextStyle;

		if (oldTextStyle.color != newTextStyle.color)
			textRenderUnit.SetColor(textStyle.color);

		if (oldTextStyle.fontManager != newTextStyle.fontManager || oldTextStyle.fontHeight != newTextStyle.fontHeight || oldTextStyle.fontName != newTextStyle.fontName || oldTextStyle.color != newTextStyle.color)
			rebuildGlyphs = true;

		cursorRenderUnit.SetStyle(Vec2f(0.08f, 0.8f) * GetFinalScale() * textStyle.fontHeight, textStyle.color, 0xffffff40);

		textStyle = newTextStyle;
	}
	void TextInput::SetEmptyTextColor(const ColorRGBAf& color)
	{
		emptyTextColor = color;

		rebuildGlyphs = true;
	}
	void TextInput::SetMultiline(bool newMultiline)
	{
		if (multiline == newMultiline)
			return;

		multiline = newMultiline;

		if (multiline)
		{
			u8String text = inputControl.GetText();
			text.Replace(u8"\n", { });
			inputControl.SetText(std::move(text));

			emptyText.Replace(u8"\n", { });
		}

		rebuildGlyphs = true;
	}
	void TextInput::SetWrapWidth(float newTextWrapWidth)
	{
		if (textWrapWidth == newTextWrapWidth)
			return;

		textWrapWidth = newTextWrapWidth;

		rebuildGlyphs = true;
	}
	void TextInput::SetValueEnteredCallback(std::function<void(u8StringView)> callback)
	{
		this->callback = callback;
	}
	bool TextInput::PreRender(const Graphics::RenderContext& renderContext)
	{
		if (rebuildGlyphs)
		{
			UpdateTextState();
			rebuildGlyphs = false;
		}

		bool oldUpdateRenderTopology = updateRenderTopology;
		updateRenderTopology = false;
		return oldUpdateRenderTopology;
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
	bool TextInput::FindWordAt(uintMem index, uintMem& begin, uintMem& end)
	{
		u8StringView text = inputControl.GetText();

		u8StringCharIterator it = { text.FirstIterator() + index, text };

		u8StringCharIterator beginIt = it;
		u8StringCharIterator endIt = it;
		
		while (beginIt != text.FirstIterator())
		{
			u8StringCharIterator prev = beginIt;
			--prev;

			UnicodeChar ch = *prev;

			if (ch.IsASCII() && isspace(static_cast<int>(ch.Value())))
				break;

			beginIt = prev;
		}

		while (endIt != text.BehindIterator())
		{
			UnicodeChar ch = *endIt;

			if (ch.IsASCII() && isspace(static_cast<int>(ch.Value())))
				break;

			++endIt;
		}

		begin = beginIt - text.FirstIterator();
		end = endIt - text.FirstIterator();
		
		return true;
	}
	bool TextInput::FindLineAt(uintMem index, uintMem& begin, uintMem& end)
	{
		u8StringView text = inputControl.GetText();

		u8StringCharIterator it = { text.FirstIterator() + index, text };

		u8StringCharIterator beginIt = it;
		u8StringCharIterator endIt = it;

		while (beginIt != text.FirstIterator())
		{
			u8StringCharIterator prev = beginIt;
			--prev;

			UnicodeChar ch = *prev;

			if (ch.Value() == U'\n')
				break;

			beginIt = prev;
		}

		while (endIt != text.BehindIterator())
		{
			UnicodeChar ch = *endIt;

			if (ch.Value() == U'\n')
				break;

			++endIt;
		}

		begin = beginIt - text.FirstIterator();
		end = endIt - text.FirstIterator();

		return true;
	}
	bool TextInput::FilterEnteredText(u8StringView text)
	{
		return true;
	}
	void TextInput::FinalScaleUpdated(const FinalScaleUpdatedEvent& event)
	{
		rebuildGlyphs = true;
		cursorRenderUnit.SetStyle(Vec2f(0.08f, 0.8f) * GetFinalScale() * textStyle.fontHeight, textStyle.color, 0xffffff40);
	}
	void TextInput::SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event)
	{
		if (inputControl.GetText().Empty())
			rebuildGlyphs = true;

		cursorRenderUnit.SetHideFlag(!IsSelected());

		if (!IsSelected())
		{
			if (inputControl.GetText() != enteredText)
			{
				if (FilterEnteredText(inputControl.GetText()))
				{
					enteredText = inputControl.GetText();

					if (callback)
						callback(enteredText);
				}
				else
				{
					inputControl.SetText(enteredText);
					rebuildGlyphs = true;
				}
			}
		}
	}
	void TextInput::MouseButtonDownEvent(const UIMouseButtonDownEvent& event)
	{
		switch (event.combo)
		{
		case 1: {
			uintMem index = layoutMetadata.HitTestCharacters(TransformFromFinalToLocalSpace(event.pos) * GetFinalScale());
			inputControl.MoveCursorTo(index, Input::GetKeyState(Input::Key::LSHIFT) || Input::GetKeyState(Input::Key::RSHIFT));
			break;
		}
		case 2: {
			uintMem begin, end;
			if (FindWordAt(inputControl.GetCursorPos(), begin, end))
			{
				inputControl.SetFocus(begin, end);
			}
			break;
		}
		case 3: {
			uintMem begin, end;
			if (FindLineAt(inputControl.GetCursorPos(), begin, end))
			{
				inputControl.SetFocus(begin, end);
			}
			break;
		}
		}

		Select();
	}
	void TextInput::MouseMotionEvent(const UIMouseMotionEvent& event)
	{
		if (IsDown())
		{
			uintMem index = layoutMetadata.HitTestCharacters(TransformFromFinalToLocalSpace(event.pos) * GetFinalScale());
			inputControl.MoveCursorTo(index, true);
		}
	}
	void TextInput::KeyDownEvent(const UIKeyDownEvent& event)
	{		
		bool shift = bool(event.modifier & Input::KeyModifier::SHIFT);
		bool ctrl = bool(event.modifier & Input::KeyModifier::CTRL);

		switch (event.key)
		{
		case Input::Key::ENTER:
			if (multiline && shift)
			{
				inputControl.InsertAtCursor("\n");
				rebuildGlyphs = true;
			}
			else
			{
				Unselect();
			}
			break;
		case Input::Key::BACKSPACE:
			if (inputControl.IsSelectionEmpty())
				inputControl.EraseBeforeCursor();
			else
				inputControl.EraseSelection();
			rebuildGlyphs = true;
			break;
		case Input::Key::DELETE:
			if (inputControl.IsSelectionEmpty())
				inputControl.EraseAfterCursor();
			else
				inputControl.EraseSelection();
			rebuildGlyphs = true;
			break;			
		case Input::Key::LEFT:
			if (inputControl.IsSelectionEmpty() || shift)
			{
				if (inputControl.GetCursorPos() != 0)
					inputControl.MoveCursorTo(inputControl.GetCursorPos() - 1, shift);
			}
			else
				inputControl.MoveCursorTo(inputControl.GetSelectionBeginning(), false);
			break;
		case Input::Key::RIGHT:
			if (inputControl.IsSelectionEmpty() || shift)
				inputControl.MoveCursorTo(inputControl.GetCursorPos() + 1, shift);
			else
				inputControl.MoveCursorTo(inputControl.GetSelectionEnd(), false);
			break;
		case Input::Key::UP:
		{
			uintMem lineIndex = layoutMetadata.GetCharacterLineIndex(inputControl.GetCursorPos());
			if (lineIndex == 0)
				break;

			Vec2f cursorPos = layoutMetadata.GetCursorPosBeforeCharacter(inputControl.GetCursorPos());

			inputControl.MoveCursorTo(layoutMetadata.HitTestLineCharacters(cursorPos, lineIndex - 1), shift);
			break;
		}
		case Input::Key::DOWN:
		{
			uintMem lineIndex = layoutMetadata.GetCharacterLineIndex(inputControl.GetCursorPos());

			if (lineIndex >= layoutMetadata.GetLineCount() - 1)
				break;

			Vec2f cursorPos = layoutMetadata.GetCursorPosBeforeCharacter(inputControl.GetCursorPos());

			inputControl.MoveCursorTo(layoutMetadata.HitTestLineCharacters(cursorPos, lineIndex + 1), shift);
			break;
		}
		case Input::Key::HOME:
		{
			uintMem lineIndex = layoutMetadata.GetCharacterLineIndex(inputControl.GetCursorPos());
			
			inputControl.MoveCursorTo(layoutMetadata.GetLineFirstCharacter(lineIndex), shift);
			break;
		}
		case Input::Key::END:
		{
			uintMem lineIndex = layoutMetadata.GetCharacterLineIndex(inputControl.GetCursorPos());

			if (lineIndex == layoutMetadata.GetLineCount() - 1)
				inputControl.MoveCursorTo(inputControl.GetText().Count(), shift);
			else
				inputControl.MoveCursorTo(layoutMetadata.GetLineFirstCharacter(lineIndex + 1) - 1, shift);
			break;
		}
		case Input::Key::A:
		{
			if (ctrl)
			{
				inputControl.MoveCursorTo(0, false);
				inputControl.MoveCursorTo(inputControl.GetText().Count(), true);
			}
			break;
		}
		case Input::Key::C:
		{
			if (ctrl)
			{
				uintMem begin = inputControl.GetSelectionBeginning();
				uintMem end = inputControl.GetSelectionEnd();

				u8StringView selectedString = inputControl.GetText().SubString(static_cast<intMem>(begin), static_cast<intMem>(end));

				Input::SetClipboardText(selectedString);
			}
			break;
		}
		case Input::Key::V:
		{
			if (ctrl)
			{
				u8String clipboardString = Input::GetClipboardText();
				inputControl.InsertAtCursor(clipboardString);
				rebuildGlyphs = true;
			}
			break;
		}
		case Input::Key::X:
		{
			if (ctrl)
			{
				uintMem begin = inputControl.GetSelectionBeginning();
				uintMem end = inputControl.GetSelectionEnd();

				u8StringView selectedString = inputControl.GetText().SubString(static_cast<intMem>(begin), static_cast<intMem>(end));

				Input::SetClipboardText(selectedString);

				inputControl.EraseSelection();
				rebuildGlyphs = true;
			}
			break;
		}
		}
	}
	void TextInput::TextInputEvent(const UITextInputEvent& event)
	{
		inputControl.InsertAtCursor(event.string);
		rebuildGlyphs = true;
	}
	void TextInput::TextInputControlStateChanged(const Text::TextCursorControl::StateChangedEvent& event)
	{
		cursorRenderUnit.Update(inputControl, layoutMetadata);
	}
	bool TextInput::IsRenderingEmptyText() const
	{
		return inputControl.GetText().Empty() && !IsSelected();
	}
	void TextInput::UpdateTextState()
	{
		bool renderEmptyText = IsRenderingEmptyText();
		u8StringView renderedText = renderEmptyText ? u8StringView(emptyText) : inputControl.GetText();

		TextStyle scaledStyle = textStyle.Scale(GetFinalScale());

		Array<Text::ShapedString> shapedLines = UpdateGlyphs(renderedText, scaledStyle);

		if (renderEmptyText)
		{
			textRenderUnit.SetColor(emptyTextColor);
			layoutMetadata.Clear();
		}
		else
		{
			textRenderUnit.SetColor(textStyle.color);
			layoutMetadata.UpdateLayoutData(shapedLines, scaledStyle);
		}

		cursorRenderUnit.Update(inputControl, layoutMetadata);
	}
	Array<Text::ShapedString> TextInput::UpdateGlyphs(u8StringView text, const TextStyle& style)
	{
		if (style.fontManager == nullptr)
			return { };

		FontManager::FontAtlasData atlasData;
		if (!style.fontManager->GetFontAtlas(textStyle.fontName, style.fontHeight, atlasData))
		{
			BLAZE_LOG_WARNING("Couldn't find font atlas with font name: \"{}\"", textStyle.fontName);
			textRenderUnit.Clear();
			return { };
		}

		updateRenderTopology = atlasData.rendererTypeID != textRenderUnit.GetRequiredRendererTypeID();

		TextInputSeparationData separationData{ text };
		TextInputTextShaper textShaper{ text, *atlasData.fontFace, static_cast<uint32>(style.fontHeight), separationData, textWrapWidth * GetFinalScale() };

		Array<Text::ShapedString> shapedLines = textShaper.Shape(text);

		Array<StaticTextRenderUnit::GlyphRenderData> glyphs = textRenderUnit.GenerateGlyphRenderData(shapedLines, *atlasData.fontFace, *atlasData.atlas, style.fontHeight, nullptr);
		textRenderUnit.SetGlyphs(std::move(glyphs), atlasData.rendererTypeID, atlasData.atlas);
		return shapedLines;
	}
}