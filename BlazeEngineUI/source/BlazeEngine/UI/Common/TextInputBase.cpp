#include "pch.h"
#include "BlazeEngine/UI/Common/TextInputBase.h"
#include "BlazeEngine/Core/String/StringCharIterator.h"
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
		TextInputTextShaper(u8StringView textContext, const Nodes::TextStyle& textStyle, const TextSeparationDataBase& textSeparationData, float maxWidth)
			: WrappedLineTextShaper(textContext, textStyle, textSeparationData, maxWidth)
		{

		}
	private:
		bool ShouldIncludeAtEnd(UnicodeChar character) override { return false; }
	};

	TextInputBase::TextInputBase()
		: updateTextRenderUnit(false), updatetCursorRenderUnit(false), inputeStringChanged(false)
	{
		cursorRenderUnit.SetHideFlag(!IsSelected());
	}
	TextInputBase::TextInputBase(Node& parent, const NodeTransform& transform)
		: TextInputBase()
	{
		SetParent(&parent);
		SetTransform(transform);
	}
	TextInputBase::TextInputBase(Node& parent, const NodeTransform& transform, TextStyle style)
		: TextInputBase(parent, transform)
	{
		SetTextStyle(std::move(style));
	}
	TextInputBase::~TextInputBase()
	{
	}
	void TextInputBase::SetTextStyle(TextStyle style)
	{
		this->style = style;
		updateTextRenderUnit = true;
	}
	void TextInputBase::SetInputedText(u8String text)
	{
		inputControl.SetText(text);
		updateTextRenderUnit = true;
		inputeStringChanged = true;
	}
	bool TextInputBase::PreRender(const Graphics::RenderContext& renderContext)
	{
		bool updateTopology = false;		

		if (updatetCursorRenderUnit || updateTextRenderUnit)
		{
			u8String renderedText = GetInputedText();
			TextStyle renderedStyle = style.Scale(GetFinalScale());

			FilterRenderData(renderedText, renderedStyle);

			if (updateTextRenderUnit)
			{
				layoutMetadata.UpdateLayoutData(ShapeText(renderedText, renderedStyle), renderedStyle);
				textRenderUnit.Initialize(layoutMetadata, renderedStyle);

				updateTopology |= renderedStyle.GetFontData().rendererTypeID != textRenderUnit.GetRequiredRendererTypeID();
				
				updateTextRenderUnit = false;
			}


			cursorRenderUnit.SetStyle(Vec2f(0.08f, 0.8f) * renderedStyle.fontHeight, renderedStyle.color, 0xffffff40_rgba);
			cursorRenderUnit.Update(inputControl, layoutMetadata);
			updatetCursorRenderUnit = false;
		}

		return updateTopology;
	}
	RenderUnitBase* TextInputBase::GetRenderUnit(uintMem index)
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
	u8StringView TextInputBase::FindWordAt(u8StringView text, uintMem index)
	{
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

		return { beginIt.Ptr(), endIt.Ptr()};
	}
	u8StringView TextInputBase::FindLineAt(u8StringView text, uintMem index)
	{
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

		return { beginIt.Ptr(), endIt.Ptr() };
	}
	bool TextInputBase::IsInsertable(UnicodeChar ch) const
	{
		return true;
	}
	void TextInputBase::TextEntered()
	{
	}
	void TextInputBase::FilterRenderData(u8String& string, TextStyle& style)
	{
	}	
	Array<Text::ShapedString> TextInputBase::ShapeText(u8StringView text, const TextStyle& style)
	{
		TextInputSeparationData separationData{ text };
		TextInputTextShaper textShaper{ text, style, separationData, FLT_MAX };
		return textShaper.Shape(text);
	}
	void TextInputBase::InvalidateRenderedText()
	{
		updateTextRenderUnit = true;
	}
	void TextInputBase::IsMouseOverChanged(Vec2f pos)
	{
		if (!IsMouseOver())
		{
			Input::SetCursorType(Input::CursorType::Default);
			return;
		}

		if (IsPressable())
			Input::SetCursorType(Input::CursorType::Text);
		else
			Input::SetCursorType(Input::CursorType::NotAllowed);
	}
	void TextInputBase::FinalScaleUpdated(const FinalScaleUpdatedEvent& event)
	{
		updateTextRenderUnit = true;		
	}
	void TextInputBase::SelectedStateChanged(const InputNode::SelectedStateChangedEvent& event)
	{
		cursorRenderUnit.SetHideFlag(!IsPressable() || !IsSelected());

		if (!IsSelected() && inputeStringChanged)
		{
			inputeStringChanged = false;

			TextEntered();
		}
	}
	void TextInputBase::MouseButtonDownEvent(const UIMouseButtonDownEvent& event)
	{
		if (!IsDown()) //This is checked to conform to the pressable flag
			return;

		switch (event.combo)
		{
		case 1:
		{
			uintMem index = layoutMetadata.HitTestCharacters(TransformFromFinalToLocalSpace(event.pos) * GetFinalScale());
			inputControl.MoveCursorTo(index, Input::GetKeyState(Input::Key::LSHIFT) || Input::GetKeyState(Input::Key::RSHIFT));
			break;
		}
		case 2:
		{
			u8StringView text = inputControl.GetText();
			u8StringView result = FindWordAt(text, inputControl.GetCursorPos());
			inputControl.SetFocus(result.FirstIterator() - text.FirstIterator(), result.BehindIterator() - text.FirstIterator());
			break;
		}
		case 3:
		{
			u8StringView text = inputControl.GetText();
			u8StringView result = FindLineAt(text, inputControl.GetCursorPos());
			inputControl.SetFocus(result.FirstIterator() - text.FirstIterator(), result.BehindIterator() - text.FirstIterator());
			break;
		}
		}

		Select();
	}
	void TextInputBase::MouseMotionEvent(const UIMouseMotionEvent& event)
	{
		if (IsDown())
		{
			uintMem index = layoutMetadata.HitTestCharacters(TransformFromFinalToLocalSpace(event.pos) * GetFinalScale());
			inputControl.MoveCursorTo(index, true);
		}
	}
	void TextInputBase::KeyDownEvent(const UIKeyDownEvent& event)
	{
		if (!IsPressable())
			return;

		bool shift = bool(event.modifier & Input::KeyModifier::SHIFT);
		bool ctrl = bool(event.modifier & Input::KeyModifier::CTRL);

		switch (event.key)
		{
		case Input::Key::ENTER:
			if (shift)
			{
				if (IsInsertable(U'\n'))
				{
					inputControl.InsertAtCursor("\n");
					updateTextRenderUnit = true;
					inputeStringChanged = true;
				}
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
			updateTextRenderUnit = true;
			inputeStringChanged = true;
			break;
		case Input::Key::DELETE:
			if (inputControl.IsSelectionEmpty())
				inputControl.EraseAfterCursor();
			else
				inputControl.EraseSelection();
			updateTextRenderUnit = true;
			inputeStringChanged = true;
			break;
		case Input::Key::LEFT:
			if (inputControl.IsSelectionEmpty() || shift)
			{
				if (inputControl.GetCursorPos() != 0)
					inputControl.MoveCursorGraphemeLeft(shift);
			}
			else
				inputControl.MoveCursorTo(inputControl.GetSelectionBeginning(), false);
			break;
		case Input::Key::RIGHT:
			if (inputControl.IsSelectionEmpty() || shift)
				inputControl.MoveCursorGraphemeRight(shift);
			else
				inputControl.MoveCursorTo(inputControl.GetSelectionEnd(), false);
			break;
		case Input::Key::UP:
		{
			auto characterLayoutData = layoutMetadata.GetCharacterData(inputControl.GetCursorPos());
			
			if (characterLayoutData.lineIndex == 0)
				break;

			Vec2f cursorPos = layoutMetadata.GetCursorPosBeforeCharacter(inputControl.GetCursorPos());

			inputControl.MoveCursorTo(layoutMetadata.HitTestLineCharacters(cursorPos, characterLayoutData.lineIndex - 1), shift);
			break;
		}
		case Input::Key::DOWN:
		{
			auto characterLayoutData = layoutMetadata.GetCharacterData(inputControl.GetCursorPos());			

			if (characterLayoutData.lineIndex >= layoutMetadata.GetLineCount() - 1)
				break;

			Vec2f cursorPos = layoutMetadata.GetCursorPosBeforeCharacter(inputControl.GetCursorPos());

			inputControl.MoveCursorTo(layoutMetadata.HitTestLineCharacters(cursorPos, characterLayoutData.lineIndex + 1), shift);
			break;
		}
		case Input::Key::HOME:
		{
			auto characterLayoutData = layoutMetadata.GetCharacterData(inputControl.GetCursorPos());
			auto lineLayoutData = layoutMetadata.GetLineData(characterLayoutData.lineIndex);			

			inputControl.MoveCursorTo(lineLayoutData.firstCharacter, shift);
			break;
		}
		case Input::Key::END:
		{
			auto characterLayoutData = layoutMetadata.GetCharacterData(inputControl.GetCursorPos());
			auto nextLineLayoutData = layoutMetadata.GetLineData(characterLayoutData.lineIndex + 1);
			
			inputControl.MoveCursorTo(nextLineLayoutData.firstCharacter - 1, shift);
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
				u8String clipboardString = FilterInsertString(Input::GetClipboardText());
				inputControl.InsertAtCursor(clipboardString);
				updateTextRenderUnit = true;
				inputeStringChanged = true;
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
				updateTextRenderUnit = true;
				inputeStringChanged = true;
			}
			break;
		}
		}
	}
	void TextInputBase::TextInputEvent(const UITextInputEvent& event)
	{
		if (!IsPressable())
			return;

		inputControl.InsertAtCursor(FilterInsertString(event.string));
		updateTextRenderUnit = true;
		inputeStringChanged = true;
	}
	void TextInputBase::TextInputControlStateChanged(const Text::TextCursorControl::StateChangedEvent& event)
	{
		updatetCursorRenderUnit = true;		
	}
	void TextInputBase::PressableFlagChanged()
	{
		Unselect();
		cursorRenderUnit.SetHideFlag(!IsPressable() || !IsSelected());
		updatetCursorRenderUnit = true;
	}
	u8String TextInputBase::FilterInsertString(u8StringView string)
	{
		u8String out;
		u8StringCharIterator it = { string.FirstIterator(), string };
		for (; it != string.BehindIterator(); ++it)
		{
			UnicodeChar ch = *it;

			if (IsInsertable(ch))
				out += ch;
		}

		return out;
	}
	bool TextInputBase::IsRenderingEmptyText() const
	{
		return inputControl.GetText().Empty() && !IsSelected();
	}
}