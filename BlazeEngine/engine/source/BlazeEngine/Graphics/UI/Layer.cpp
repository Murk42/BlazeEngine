#include "BlazeEngine/Graphics/UI/Layer.h"
#include "BlazeEngine/Graphics/UI/Elements/Text.h"
#include "BlazeEngine/Graphics/UI/Elements/Panel.h"
#include "BlazeEngine/Graphics/UI/States/ButtonState.h"
#include "BlazeEngine/Graphics/UI/States/TextFieldState.h"

#include "BlazeEngine/Graphics/UI/LayerManager.h"

namespace Blaze::UI
{	
	void Layer::Add(Text& text)
	{
		textRenderer.AddText(text);
		elementManager.AddElement(text);
	}

	void Layer::Add(Panel& panel)
	{
		panelRenderer.AddPanel(panel);
		elementManager.AddElement(panel);
	}

	void Layer::Add(ButtonState& button)
	{
		buttonManager.AddButton(button);
		stateManager.AddState(button);
	}

	void Layer::Add(TextFieldState& textField)
	{
		textFieldManager.AddTextField(textField);
		stateManager.AddState(textField);
	}

	Layer::Layer()
		: manager(nullptr)
	{		
		elementManager.layer = this;
		stateManager.layer = this;

		textFieldManager.stateManager = &stateManager;
	}

	void Layer::SetActive(bool active)
	{
		this->active = active;
		if (active)		
			Activated();										
	}

	void Layer::SetInteractable(bool interactable)
	{
		this->interactable = interactable;
	}
}