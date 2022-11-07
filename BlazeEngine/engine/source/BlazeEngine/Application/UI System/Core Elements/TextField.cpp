#include "BlazeEngine/Application/UI System/Core Elements/TextField.h"
#include "BlazeEngine/Input/Input.h"

namespace Blaze::UI
{
	void TextField::OnEvent(Event::TextInput event)
	{
		Insert(event.input);
	}	
	void TextField::UpdateText()
	{		
		if (text.CharacterCount() == 0 && !focused)
		{
			button.SetTextAlignment(Align::Center, Align::Center, Vec2f(0, 0));
			button.SetText(emptyText);
			button.SetTextColor(0x999999ff);
		}
		else
		{
			button.SetTextAlignment(Align::Left, Align::Left, Vec2f(5, 0));
			button.SetText(text);
			button.SetTextColor(0xffffffff);
		}
	}
	void TextField::UpdateCursor()
	{
		if (this->cursorPos == 0)
			cursor.SetPos({ -1, 0 });
		else
		{
			auto v = button.GetTextElement().GetVertices()[this->cursorPos - 1];
			cursor.SetPos({ v.next - 1, 0 });
		}
	
		float textSize = button.GetTextElement().GetSize().x;
		float buttonSize = button.GetSize().x - 10;
		float cursorPos = cursor.GetPos().x;

		if (this->cursorPos == text.CharacterCount() && text.CharacterCount() != 0)
			textSize = button.GetTextElement().GetVertices().back().next;

		if (textSize < buttonSize)		
			button.SetTextOffset({ 5, 0 });							//Aligned to the left
		else if (textSize - cursorPos < buttonSize / 2)
			button.SetTextOffset({ buttonSize - textSize + 5 - cursor.GetSize().x, 0 }); //Aligned to the right
		else if (cursorPos < buttonSize / 2)
			button.SetTextOffset({ 5, 0 });							//ALigned to the left so the cursor is inside
		else
			button.SetTextOffset({ buttonSize / 2 - cursorPos + 5, 0});	//Aligned so the cursor is in the middle
	}
	void TextField::Erase()
	{
		if (cursorPos == 0)
			return;

		StringUTF8 prefix = text.SubString(0, cursorPos - 1);
		StringUTF8 sufix = text.SubString(cursorPos, text.CharacterCount() - cursorPos);		
		StringUTF8 newText = prefix + sufix;

		if (validateTextFunction == nullptr || validateTextFunction(newText))
		{
			text = newText;
			--cursorPos;
			UpdateText();
			UpdateCursor();
		}
	}
	void TextField::Insert(StringViewUTF8 x)
	{
		StringUTF8 prefix = text.SubString(0, cursorPos);
		StringUTF8 sufix = text.SubString(cursorPos, text.CharacterCount() - cursorPos);		
		StringUTF8 newText = prefix + x + sufix;

		if (validateTextFunction == nullptr || validateTextFunction(newText))
		{
			text = newText;
			
			++cursorPos;
			UpdateText();
			UpdateCursor();
		}
	}
	void TextField::MoveLeft()
	{
		cursorPos--;
		cursorPos = std::clamp((int)cursorPos, 0, (int)text.CharacterCount());
		UpdateCursor();
	}
	void TextField::MoveRight()
	{
		cursorPos++;
		cursorPos = std::clamp((int)cursorPos, 0, (int)text.CharacterCount());
		UpdateCursor();
	}	

	TextField::TextField()
		: focused(false), emptyText(), cursorPos(0), EventHandler<Event::TextInput>()
	{
		TieElement(&button);
		TieElement(&cursor);

		button.SetAnchor(this);

		button.SetSolidFlag(false);
		

		button.AddPressedFunction([&]() {
			if (!focused)
			{
				Focus();				
			}

			if (focused)
			{
				Vec2f offset = button.GetTextElement().GetViewportPos();
				Vec2f mpRel = (Vec2f)Input::GetMousePos() - (Vec2f)GetManager()->GetViewport().pos - offset;
				auto& vertices = button.GetTextElement().GetVertices();
				uint i = 0;

				Vec2f prev = Vec2f();

				for (; i < vertices.size(); ++i)
					if (mpRel.x < (prev.x + vertices[i].next) / 2)
						break;
					else
						prev.x = vertices[i].next;

				cursorPos = i;
				UpdateCursor();
			}
			});

		cursor.SetPos(Vec2f(1, 0));
		cursor.SetSize(Vec2f(2, 0));
		cursor.SetAnchor(&button.GetTextElement());
		cursor.SetLocalAlignment(Align::Left);
		cursor.SetAnchorAlignment(Align::Left);
		cursor.SetActiveFlag(false);
		cursor.SetSolidFlag(false);
		cursor.borderWidth = 0;		
		cursor.fillColor = 0xffffffff;
		SetFontSize(button.GetFontSize());

		UpdateText();
	}

	void TextField::SetSize(Vec2f size)
	{
		button.SetSize(size);
		UIElement::SetSize(size);
	}
	void TextField::SetActiveFlag(bool active)
	{
		if (!active && focused)		
			ReturnFocus();

		button.SetActiveFlag(active);
		UIElement::SetActiveFlag(active);
	}	
	void TextField::SetDepth(float value)
	{
		button.SetDepth(value);
		UIElement::SetDepth(value);
	}	
	void TextField::SetBorderWidth(float width)
	{
		button.SetBorderWidth(width);
	}
	void TextField::SetCornerRadius(float radius)
	{
		button.SetCornerRadius(radius);
	}
	void TextField::SetNormalColor(ColorRGBAf color)
	{
		button.SetNormalColor(color);
	}
	void TextField::SetHoveredColor(ColorRGBAf color)
	{
		button.SetHoveredColor(color);
	}
	void TextField::SetPressedColor(ColorRGBAf color)
	{
		button.SetPressedColor(color);
	}
	void TextField::SetBorderColor(ColorRGBAf color)
	{
		button.SetBorderColor(color);
	}
	void TextField::SetFontResolution(FontResolution* fontResolution)
	{
		button.SetFontResolution(fontResolution);
	}
	void TextField::SetFontSize(size_t fontSize)
	{
		button.SetFontSize(fontSize);
		cursor.SetSize(Vec2f(std::ceil(fontSize * 0.07f), std::ceil(fontSize * 0.7f)));
		cursor.cornerRadius = cursor.GetSize().x / 2;
	}
	void TextField::SetText(StringViewUTF8 newText)
	{
		if (validateTextFunction == nullptr || validateTextFunction(newText))
		{
			text = newText;

			UpdateText();			
		}
	}

	void TextField::SetEmptyText(StringViewUTF8 text)
	{
		this->emptyText = text;
		if (this->text.CharacterCount() == 0)
			button.SetText(emptyText);
	}

	bool TextField::Focus()
	{
		if (GetManager()->TakeFocus(this))
		{
			focused = true;
			
			cursorPos = text.CharacterCount();

			EventManager::AddEventHandler<Event::TextInput>(*this);			

			cursor.SetActiveFlag(true);
			focusTime = TimePoint::GetWorldTime();

			UpdateText();

			return true;
		}
		return false;
	}

	void TextField::ReturnFocus()
	{		
		if (focused)
		{
			GetManager()->AddEventToQueue(valueEntered);
			focused = false;

			EventManager::RemoveEventHandler<Event::TextInput>(*this);

			GetManager()->ReturnFocus();

			cursor.SetActiveFlag(false);

			UpdateText();
		}
	}

	void TextField::SetProperties(const TextFieldProperties& p)
	{	
		if (p.buttonProperties.textProperties.fontSize.edited)		
			cursor.SetSize(Vec2f(2, p.buttonProperties.textProperties.fontSize * 0.7f));
		
		button.SetProperties(p.buttonProperties);
		if (p.text.edited) SetText(p.text.value);
		if (p.emptyText.edited) SetEmptyText(p.emptyText.value);		
		if (p.validateTextFunction.edited) SetValidateTextFunction(p.validateTextFunction);
		if (p.valueEntered.edited) AddValueEnteredFunction(p.valueEntered);
	}

	void TextFieldManager::Update(UIElement* element)
	{
		Vec2i mp = Input::GetMousePos() - GetManager()->GetViewport().pos;
		auto clickData = Input::GetKeyState(MouseKey::MouseLeft);
		auto backspace = Input::GetKeyState(Key::Backspace);
		TimePoint now = TimePoint::GetWorldTime();

		TextField& el = (TextField&)*element;		

		Vec2f  p1 = el.GetViewportPos();
		Vec2f p2 = p1 + el.GetSize();

		bool inside = 
			mp.x > p1.x && mp.x < p2.x&&
			mp.y > p1.y && mp.y < p2.y;

		if (el.focused && Input::GetKeyState(Key::Return).pressed || !inside && clickData.pressed)
		{
			el.ReturnFocus();
		}
		if (el.focused)
		{ 
			if (Input::GetKeyState(Key::Home).pressed)
			{
				el.cursorPos = 0;
				el.UpdateCursor();
			}
			if (Input::GetKeyState(Key::End).pressed)
			{
				el.cursorPos = el.text.CharacterCount();
				el.UpdateCursor();
			}

			if (fmod(now - el.focusTime, 1.0) < 0.5f)
				el.cursor.SetActiveFlag(true);
			else
				el.cursor.SetActiveFlag(false);

			if (Input::GetKeyState(Key::Left).pressed)
				el.MoveLeft();
			if (Input::GetKeyState(Key::Right).pressed)
				el.MoveRight();

			if (backspace.pressed)
			{
				el.backspaceTime = now;
				el.backspaceCount = 0;
			}

			if (backspace.down || backspace.pressed)
				if (now - el.backspaceTime < 0.6f)
				{
					if (el.backspaceCount <= (now - el.backspaceTime) / 0.2f && el.text.CharacterCount() != 0)
					{
						el.backspaceCount++;
						el.Erase();
					}
				}
				else
				{
					if (el.backspaceCount - 3 < (now - el.backspaceTime - 0.6f) / 0.05f && el.text.CharacterCount() != 0)
					{
						el.backspaceCount++;
						el.Erase();
					}
				}
		}

		GetManager()->GetElementManager<UI::TextButton>().Update(&el.button);
		GetManager()->GetElementManager<UI::Panel>().Update(&el.cursor);
	}
	void TextFieldManager::Render(UIElement* ptr)
	{
		TextField& el = *(TextField*)ptr;
		GetManager()->GetElementManager<UI::TextButton>().Render(&el.button);
		GetManager()->GetElementManager<UI::Panel>().Render(&el.cursor);
	}
}