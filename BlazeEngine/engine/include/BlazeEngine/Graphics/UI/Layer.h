#pragma once
#include "BlazeEngine/Core/EngineCore.h"

#include "BlazeEngine/Graphics/UI/Elements/ElementManager.h"
#include "BlazeEngine/Graphics/UI/States/StateManager.h"

#include "BlazeEngine/Graphics/UI/Elements/TextRenderer.h"
#include "BlazeEngine/Graphics/UI/Elements/PanelRenderer.h"

#include "BlazeEngine/Graphics/UI/States/TextFieldStateManager.h"
#include "BlazeEngine/Graphics/UI/States/ButtonStateManager.h"

namespace Blaze
{
	class Engine;
}
namespace Blaze::UI
{
	class LayerManager;

	class BLAZE_API Layer
	{				
	protected:
		virtual void Setup() { }
		virtual void Update() { }
		virtual void NewViewport(Vec2i size) { }
		virtual void Activated() { }		

		void Add(Text&);
		void Add(Panel&);
		void Add(ButtonState&);
		void Add(TextFieldState&);		

		ElementManager elementManager;
		StateManager stateManager;

		TextRenderer textRenderer;
		PanelRenderer panelRenderer;

		ButtonStateManager buttonManager;
		TextFieldStateManager textFieldManager;

		bool active = true;
		bool interactable = true;
	public:
		Layer();

		void SetActive(bool active);
		void SetInteractable(bool interactable);		
		
		friend class Element;
		friend class State;
		friend class Blaze::Engine;
	};
}