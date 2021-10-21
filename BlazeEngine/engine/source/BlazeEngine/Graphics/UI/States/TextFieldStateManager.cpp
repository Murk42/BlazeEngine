#include "BlazeEngine/Graphics/UI/States/TextFieldStateManager.h"
#include "BlazeEngine/Graphics/UI/States/TextFieldState.h"
#include "BlazeEngine/Graphics/UI/States/StateManager.h"
#include "BlazeEngine/Graphics/UI/Elements/Element.h"
#include "BlazeEngine/DataStructures/Vector.h"

namespace Blaze::UI
{
	bool IsInside(Vec2f pos, Vec2f size);	

	TextFieldStateManager::TextFieldStateManager()
		: stateManager(nullptr)
	{
	}

	void TextFieldStateManager::AddTextField(TextFieldState& textField)
	{
		textFields.emplace_back(&textField);
	}

	void TextFieldStateManager::MouseMotionEvent(InputEvent::MouseMotion event)
	{
		for (auto& textField : textFields)
		{
			bool over = IsInside(event.pos - textField->parent->transform.absolutePos, textField->parent->transform.absoluteSize);

			if (textField->hovered != over)
			{
				textField->hovered = over;
				if (textField->StateChangedEvent)
					textField->StateChangedEvent();
			}
			textField->hovered = over;
		}
	}

	void TextFieldStateManager::MousePressedEvent(InputEvent::MousePressed event)
	{
		for (auto& textField : textFields)
		{
			textField->down = IsInside(event.pos - textField->parent->transform.absolutePos, textField->parent->transform.absoluteSize);

			if (textField->down)			
				stateManager->Select(*textField);			
		}
	}

	void TextFieldStateManager::MouseReleasedEvent(InputEvent::MouseReleased event)
	{
		for (auto& textField : textFields)
		{
			if (textField->down)
			{
				textField->down = false;

				if (textField->StateChangedEvent)
					textField->StateChangedEvent();
			}
		}
	}

	void TextFieldStateManager::KeyPressedEvent(InputEvent::KeyPressed event)
	{
		if (stateManager->GetSelected() != nullptr)
		{
			TextFieldState* textField = nullptr;

			for (auto& tf : textFields)
				if (tf == stateManager->GetSelected())
					textField = tf;

			if (textField == nullptr)
				return;

			switch (event.key)
			{
			case Key::Backspace:
				if (textField->string.Size() > 0)
				{
					size_t size = UTF8Size(textField->string.Ptr() + textField->string.Size() - 1);
					textField->string.Resize(textField->string.Size() - size);

					if (textField->StringChangedEvent)
						textField->StringChangedEvent();
				}
				break;
			case Key::Return:
				if (textField->StringFinishedEvent)
					textField->StringFinishedEvent();

				stateManager->Deselect();

				if (textField->StateChangedEvent)
					textField->StateChangedEvent();
				if (textField->DeselectedEvent)
					textField->DeselectedEvent();
				break;
			}
		}
	}

	void TextFieldStateManager::TextInputEvent(InputEvent::TextInput event)
	{
		if (stateManager->GetSelected() != nullptr)
		{
			TextFieldState* textField = nullptr;

			for (auto& tf : textFields)
				if (tf == stateManager->GetSelected())
					textField = tf;

			if (textField == nullptr)
				return;

			textField->string = textField->string + event.string;
			if (textField->StringChangedEvent)
				textField->StringChangedEvent();
		}
	}
}