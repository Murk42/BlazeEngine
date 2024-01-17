#include "pch.h"
#include "BlazeEngineGraphics/UI/Elements/UIInputBox.h"
#include "BlazeEngineGraphics/UI/Input/UIInputManager.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	InputBox::InputBox(Node* parent, const NodeTransform& transform)
		: UIInputNode(parent, transform), 
		panelNode(this, { }), textCullingNode(this, {}), textNode(&textCullingNode, { }), cursorNode(&textNode, {}),
		renderStream(this), emptyText("Enter text"),
		selected(false), selectedTime(0), mouseDown(false)
	{
		SetHitPropagation(true);
		transformUpdatedEventDispatcher.AddHandler(*this);

		textCullingNode.SetTransform({
			.pos = Vec2f(), 
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),
			.size = Vec2f(), //Set in TransformPreUpdateEvent
			.scale = 1.0f,
			.rotation = 0.0f
			});

		cursorNode.renderStream = &renderStream;
		cursorNode.SetTransform({
			.pos = Vec2f(), //Set in UpdateCursor
			.parentPivot = Vec2f(0.0f, 0.5f),
			.pivot = Vec2f(0.0f, 0.5f),
			.size = Vec2f(), //Set in UpdateCursor
			.scale = 1.0f,
			.rotation = 0.0f
			});
				
		textNode.SetLayoutOptions(Text::HorizontalLayout::ChangeSize, Text::VerticalLayout::ChangeSize, Text::LineAlign::Left, false);
		textNode.cullingNode = &textCullingNode;
		textNode.SetFontHeight(12);
		textNode.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),			
			.scale = 1.0f,
			.rotation = 0.0f
			});

		panelNode.fillColor = ColorRGBAf(0x333333ff);
		panelNode.borderColor = ColorRGBAf(0x111111ff);
		panelNode.borderWidth = 0.0f;
		panelNode.cornerRadius = 5.0f;
		panelNode.SetTransform({
			.pos = Vec2f(0, 0),
			.parentPivot = Vec2f(0.5f, 0.5f),
			.pivot = Vec2f(0.5f, 0.5f),
			.size = transform.size,
			.scale = 1.0f,
			.rotation = 0.0f
		});			

		UpdateCursorAndText();
		UpdatePanelColor();
	}	
	void InputBox::SetText(StringViewUTF8 text)
	{
		this->text = text;
		UpdateCursorAndText();
	}
	void InputBox::SetEmptyText(StringViewUTF8 text)
	{
		emptyText = text;
		UpdateCursorAndText();
	}
	StringViewUTF8 InputBox::GetText()
	{
		return text;
	}
	void InputBox::SetNormalColor(ColorRGBAf color)
	{
		panelNormalColor = color;
		UpdatePanelColor();
	}
	void InputBox::SetHighlightedColor(ColorRGBAf color)
	{
		panelHighlightedColor = color;
		UpdatePanelColor();
	}
	void InputBox::SetPressedColor(ColorRGBAf color)
	{
		panelPressedColor = color;
		UpdatePanelColor();
	}
	void InputBox::SetTextEmptyColor(ColorRGBAf color)
	{
		textEmptyColor = color;
		UpdateTextColor();
	}
	void InputBox::SetTextEditingColor(ColorRGBAf color)
	{
		textEditingColor = color;
		UpdateTextColor();
	}
	void InputBox::SetTextNormalColor(ColorRGBAf color)
	{
		textNormalColor = color;
		UpdateTextColor();
	}
	void InputBox::SetTextAlign(TextAlign align)
	{
		textAlign = align;

		UpdateTransform();
	}
	void InputBox::UpdatePanelColor()
	{
		if (pressed)
			panelNode.fillColor = panelPressedColor;
		else if (hovered)
			panelNode.fillColor = panelHighlightedColor;
		else
			panelNode.fillColor = panelNormalColor;
	}
	void InputBox::UpdateTextColor()
	{
		if (this->text.CharacterCount() == 0 && !selected)
			textNode.SetTextColor(textEmptyColor);
		else if (selected)
			textNode.SetTextColor(textNormalColor);
		else
			textNode.SetTextColor(textEditingColor);
	}	
	void InputBox::UpdateCursorAndText()
	{						
		if (this->text.CharacterCount() == 0 && !selected)
		{
			textNode.SetText(emptyText);
			textNode.SetTextColor(textEmptyColor);
		}
		else
		{
			if (selected)
				textNode.SetTextColor(textNormalColor);
			else
				textNode.SetTextColor(textEditingColor);

			textNode.SetText(text);
		}

		//Generates the character data and resizes the node
		textNode.ForceUpdateTransform();

		auto align = textAlign;

		auto textTransform = textNode.GetTransform();

		if (selected)
		{
			auto cursorTransform = cursorNode.GetTransform();
			auto textCullingTransform = textCullingNode.GetTransform();

			cursorTransform.pos.x = GetCharacterAdvance(cursorPos) - 1;
			cursorTransform.pos.y = 0;
			cursorTransform.size = Vec2f(1, textNode.GetPixelFontHeight() * 0.8f);

			if (textTransform.size.x > textCullingTransform.size.x)
			{
				align = TextAlign::Left;

				/*
				
				_ represents the cursor
				<=============================textTransform.size.x================================>
				                                                <==textCullingTransform.size.x==>
				This is a test sentence and it shows some things|and it demo_strates how this wo|ks
				<===========textTransform.pos.x==================
				===============textViewOffset===================>

				=================leftTextViewOffset=============>

				=================leftTextViewOffset=============>==textCullingTransform.size.x==>
				============================rightTextViewOffset=================================>
				*/

				float leftTextViewOffset = textViewOffset;
				float rightTextViewOffset = std::min(leftTextViewOffset + textCullingTransform.size.x, textTransform.size.x);

				if (selected)
				{
					if (cursorTransform.pos.x < leftTextViewOffset)
						textViewOffset = cursorTransform.pos.x;
					if (cursorTransform.pos.x > rightTextViewOffset)
						textViewOffset = cursorTransform.pos.x - textCullingTransform.size.x;
					if (textViewOffset > cursorTransform.pos.x - textCullingTransform.size.x / 2)
						textViewOffset = cursorTransform.pos.x - textCullingTransform.size.x / 2;

					textTransform.pos.x = -textViewOffset;
				}
				else
					textTransform.pos.x = 0.0f;
			}
			else
			{
				textTransform.pos.x = 0;
				textViewOffset = 0.0f;
			}

			cursorNode.SetTransform(cursorTransform);
		}
		
		switch (align)
		{
		case InputBox::TextAlign::Left:
			textTransform.parentPivot = Vec2f(0.0f, 0.5f);
			textTransform.pivot = Vec2f(0.0f, 0.5f);			
			break;	
		default:
		case InputBox::TextAlign::Center:
			textTransform.parentPivot = Vec2f(0.5f, 0.5f);
			textTransform.pivot = Vec2f(0.5f, 0.5f);			
			break;
		case InputBox::TextAlign::Right:
			textTransform.parentPivot = Vec2f(1.0f, 0.5f);
			textTransform.pivot = Vec2f(1.0f, 0.5f);			
			break;
		}

		textNode.SetTransform(textTransform);
	}
	uint InputBox::GetScreenPositionClosestCharacterBarrier(Vec2f targetPos)
	{		
		Vec2f pos = textNode.GetFinalPosition();
		float rotation = textNode.GetFinalRotation();
		float finalScale = textNode.GetFinalScale();
		float cos = Math::Cos(rotation);
		float sin = Math::Sin(rotation);
		Vec2f right = Vec2f(cos, sin);

		float advance = right.DotProduct(targetPos - pos);

		auto characterData = textNode.GetCharacterData();

		if (text.Empty())
			return 0;

		if (advance < (characterData.First().pos.x + characterData.First().size.x / 2) * textNode.GetFinalScale())
			return 0;

		for (uint i = 1; i < characterData.Count(); ++i)
			if (advance < (characterData[i].pos.x + characterData[i].size.x / 2) * textNode.GetFinalScale())
				return i;

		return characterData.Count();
	}
	uint InputBox::GetScreenPositionClosestCharacter(Vec2f targetPos)
	{
		Vec2f pos = textNode.GetFinalPosition();
		float rotation = textNode.GetFinalRotation();
		float finalScale = textNode.GetFinalScale();
		float cos = Math::Cos(rotation);
		float sin = Math::Sin(rotation);
		Vec2f right = Vec2f(cos, sin);

		float advance = right.DotProduct(targetPos - pos);

		auto characterData = textNode.GetCharacterData();

		if (text.Empty())
			return 0;

		if (text.CharacterCount() == 1)
			return 0;

		for (uint i = 0; i < characterData.Count() - 1; ++i)
			if (advance < (characterData[i].pos.x + characterData[i].size.x + characterData[i + 1].pos.x) / 2 * textNode.GetFinalScale())
				return i;

		return characterData.Count() - 1;		
	}
	float InputBox::GetCharacterAdvance(uint characterIndex)
	{
		if (text.Empty())
			return 0.0f;

		auto characterData = textNode.GetCharacterData();
		if (characterIndex >= characterData.Count())
			return textNode.GetTransform().size.x + 1;
		else if (characterIndex == 0)
			return characterData.First().pos.x;
		else
			return std::ceil((characterData[characterIndex].pos.x + characterData[characterIndex - 1].pos.x + characterData[characterIndex - 1].size.x) / 2);
	}
	void InputBox::FindWord(uint index, uint& begin, uint& end)
	{
		if (text.Empty())
		{
			begin = index;
			end = index;
			return;
		}

		begin = 0;
		end = 0;

		int wordType = INT_MAX;
		for (auto it = text.FirstIterator(); it != text.BehindIterator(); ++it)
		{
			char value = it.ToUnicode().Value();

			int newWordType = 0;

			if (isalpha(value) || isdigit(value) && wordType == 0)
				newWordType = 0;
			else if (isdigit(value))
				newWordType = 1;
			else
				newWordType = 2;

			bool newWord = wordType != newWordType && wordType != INT_MAX;

			if (newWord)
			{
				if (index < end)
					return;

				begin = end;			
			}			

			++end;

			wordType = newWordType;
		}		
	}
	void InputBox::OnEvent(NodeTransformUpdatedEvent event)
	{
		auto transform = GetTransform();

		auto textCullingTransform = textCullingNode.GetTransform();
		textCullingTransform.size = transform.size - Vec2f(12, 2);
		textCullingNode.SetTransform(textCullingTransform);
		
		auto panelTransform = panelNode.GetTransform();
		panelTransform.size = transform.size;
		panelNode.SetTransform(panelTransform); 
	}	
	void InputBox::OnEvent(SelectedEvent event)
	{
		selectedTime = TimePoint::GetRunTime();
		selected = true;
		cursorPos = text.CharacterCount();
		selectionBeginPos = cursorPos;
		selectionEndPos = cursorPos;

		UpdateCursorAndText();
		UpdateTextColor();
	}
	void InputBox::OnEvent(DeselectedEvent event)
	{
		pressed = false;
		selected = false;		

		UpdatePanelColor();
		UpdateTextColor();

		Input::SetCursorType(Input::CursorType::Arrow);
	}
	void InputBox::OnEvent(KeyPressedEvent event)
	{
		switch (event.key)
		{
		case Key::Enter:
			event.inputManager->SelectNode(nullptr);
			UpdateCursorAndText();
			break;
		case Key::Backspace:
			if (selectionBeginPos == selectionEndPos)
			{
				if (cursorPos == 0)
					break;

				text.EraseSubString(cursorPos - 1, 1);

				--cursorPos;
			}
			else
			{				
				text.EraseSubString(selectionBeginPos, selectionEndPos - selectionBeginPos);

				cursorPos = selectionBeginPos;

				selectionEndPos = selectionBeginPos;				
			}
			
			selectedTime = TimePoint::GetRunTime();

			UpdateCursorAndText();
			break;
		case Key::Delete:
			if (selectionBeginPos == selectionEndPos)
			{
				if (cursorPos == text.CharacterCount())
					break;

				text.EraseSubString(cursorPos, 1);				
			}
			else
			{
				text.EraseSubString(selectionBeginPos, selectionEndPos - selectionBeginPos);

				cursorPos = selectionBeginPos;

				selectionEndPos = selectionBeginPos;
			}

			selectedTime = TimePoint::GetRunTime();

			UpdateCursorAndText();
			break;
		case Key::Left:

			if (Input::GetLastKeyState(Key::LShift).down)
			{
				if (cursorPos == 0)
					break;

				if (cursorPos == selectionBeginPos)
				{
					--cursorPos;
					selectionBeginPos = cursorPos;
				}
				else if (cursorPos == selectionEndPos)
				{
					--cursorPos;
					selectionEndPos = cursorPos;
				}

				selectedTime = TimePoint::GetRunTime();
			}
			else
			{
				if (cursorPos == 0)
				{
					selectionBeginPos = cursorPos;
					selectionEndPos = cursorPos;
					break;
				}

				if (selectionBeginPos == selectionEndPos)
				{
					--cursorPos;
					selectedTime = TimePoint::GetRunTime();
				}

				selectionBeginPos = cursorPos;
				selectionEndPos = cursorPos;
			}			

			UpdateCursorAndText();
			break;
		case Key::Right:

			if (Input::GetLastKeyState(Key::LShift).down)
			{
				if (cursorPos == text.CharacterCount())
					break;

				if (cursorPos == selectionEndPos)
				{
					++cursorPos;
					selectionEndPos = cursorPos;
				}
				else if (cursorPos == selectionBeginPos)
				{
					++cursorPos;
					selectionBeginPos = cursorPos;
				}

				selectedTime = TimePoint::GetRunTime();
			}
			else
			{
				if (cursorPos == text.CharacterCount())
				{
					selectionBeginPos = cursorPos;
					selectionEndPos = cursorPos;
					break;
				}
				else
				{
					if (selectionBeginPos == selectionEndPos)
					{
						++cursorPos;
						selectedTime = TimePoint::GetRunTime();
					}

					selectionBeginPos = cursorPos;
					selectionEndPos = cursorPos;
				}
			}

			UpdateCursorAndText();
			break;
		case Key::Home:
			if (Input::GetLastKeyState(Key::LShift).down)
			{
				if (selectionBeginPos < cursorPos)
				{
					selectionEndPos = selectionBeginPos;
					selectionBeginPos = 0;
				}
				else
				{
					if (selectionBeginPos == selectionEndPos)
						selectionEndPos = cursorPos;
					selectionBeginPos = 0;
				}
			}
			else
			{
				if (selectionBeginPos != selectionEndPos)
				{
					selectionBeginPos = cursorPos;
					selectionEndPos = cursorPos;
					break;
				}

				selectionBeginPos = 0;
				selectionEndPos = 0;
			}

			cursorPos = 0;

			UpdateCursorAndText();
			break;
		case Key::End:			
			if (Input::GetLastKeyState(Key::LShift).down)
			{
				if (selectionEndPos > cursorPos)
				{
					selectionBeginPos = selectionEndPos;
					selectionEndPos = text.CharacterCount();
				}
				else
				{
					if (selectionBeginPos == selectionEndPos)
						selectionBeginPos = cursorPos;
					selectionEndPos = text.CharacterCount();
				}
			}
			else
			{
				if (selectionBeginPos != selectionEndPos)
				{
					selectionBeginPos = cursorPos;
					selectionEndPos = cursorPos;
					break;
				}

				selectionBeginPos = text.CharacterCount();
				selectionEndPos = text.CharacterCount();

			}

			cursorPos = text.CharacterCount();

			UpdateCursorAndText();
			break;
		}
	}
	void InputBox::OnEvent(TextInputEvent event)
	{
		if (event.inputManager->GetSelectedNode() == this)
		{			
			if (selectionBeginPos != selectionEndPos)
			{
				text.EraseSubString(selectionBeginPos, selectionEndPos - selectionBeginPos);
				cursorPos = selectionBeginPos;
			}

			text.InsertString(cursorPos, event.input);			
			cursorPos += event.input.CharacterCount();
			selectionBeginPos = cursorPos;
			selectionEndPos = cursorPos;
				
			UpdateCursorAndText();
		}
	}
	void InputBox::OnEvent(MousePressedEvent event)
	{
		cursorPos = GetScreenPositionClosestCharacterBarrier(event.pos);

		if (event.combo == 3)
		{
			mouseDownSelectionBeginPos = 0;
			mouseDownSelectionEndPos = text.CharacterCount();
			cursorPos = mouseDownSelectionEndPos;
		}
		else if (event.combo == 2)
		{									
			FindWord(GetScreenPositionClosestCharacter(event.pos),
				mouseDownSelectionBeginPos,
				mouseDownSelectionEndPos
			);			
			cursorPos = mouseDownSelectionEndPos;
		}
		else if (event.combo == 1)
		{
			mouseDownSelectionBeginPos = cursorPos;
			mouseDownSelectionEndPos = cursorPos;
		}

		selectionBeginPos = mouseDownSelectionBeginPos;
		selectionEndPos = mouseDownSelectionEndPos;

		if (!selected)
		{
			pressed = true;
			UpdatePanelColor();
		}
		else
			mouseDown = true;

		event.inputManager->SelectNode(this);

		Input::SetCursorType(Input::CursorType::IBeam);

		UpdateCursorAndText();
	}
	void InputBox::OnEvent(MouseMotionEvent event)
	{
		if (mouseDown)
		{
			cursorPos = GetScreenPositionClosestCharacterBarrier(event.pos);
			
			if (cursorPos < mouseDownSelectionBeginPos)
			{
				selectionEndPos = mouseDownSelectionEndPos;
				selectionBeginPos = cursorPos;
			}
			else if (cursorPos > mouseDownSelectionEndPos)
			{
				selectionBeginPos = mouseDownSelectionBeginPos;
				selectionEndPos = cursorPos;				
			}
			else
			{
				selectionBeginPos = mouseDownSelectionBeginPos;
				selectionEndPos = mouseDownSelectionEndPos;
				cursorPos = mouseDownSelectionEndPos;
			}

			UpdateCursorAndText();
		}
	}
	void InputBox::OnEvent(MouseReleasedEvent event)
	{
		if (mouseDown)
		{
			mouseDown = false;
			cursorPos = GetScreenPositionClosestCharacterBarrier(event.pos);

			if (cursorPos < mouseDownSelectionBeginPos)
			{
				selectionEndPos = mouseDownSelectionEndPos;
				selectionBeginPos = cursorPos;
			}
			else if (cursorPos > mouseDownSelectionEndPos)
			{
				selectionBeginPos = mouseDownSelectionBeginPos;
				selectionEndPos = cursorPos;
			}
			else
			{
				selectionBeginPos = mouseDownSelectionBeginPos;
				selectionEndPos = mouseDownSelectionEndPos;
				cursorPos = mouseDownSelectionEndPos;
			}

			UpdateCursorAndText();
		}

		pressed = false;
		hovered = false;
		UpdatePanelColor();
	}
	void InputBox::OnEvent(MouseEnterEvent event)
	{		
		if (selected)
		{
			Input::SetCursorType(Input::CursorType::IBeam);			
		}
		else
		{
			hovered = true;
			UpdatePanelColor();
		}

	}
	void InputBox::OnEvent(MouseExitEvent event)
	{
		if (selected)		
			Input::SetCursorType(Input::CursorType::Arrow);				
		else
		{
			hovered = false;
			UpdatePanelColor();
		}
	}	
}