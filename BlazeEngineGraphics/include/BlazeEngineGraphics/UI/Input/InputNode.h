#pragma once
#include "BlazeEngineCore/Event/EventHandler.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI
{
	class UIKeyboardEventHandler;
	class UIMouseEventHandler;
	class UISelectEventHandler;
	class InputManager;

	class BLAZE_GRAPHICS_API InputNode : 
		public Node,
		private EventHandler<Node::FinalTransformUpdatedEvent>,
		private EventHandler<Node::EnabledStateUpdatedEvent>
	{
	public:
		InputNode();
		~InputNode() override;

		void SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler);
		void SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler);
		void SetSelectEventHandler(UISelectEventHandler* selectEventHandler);		
	private:			
		int hit;
		InputManager* inputManager;

		UIKeyboardEventHandler* keyboardHandler;
		UIMouseEventHandler* mouseHandler;
		UISelectEventHandler* selectHandler;

		void OnEvent(const Node::FinalTransformUpdatedEvent& event) override;
		void OnEvent(const Node::EnabledStateUpdatedEvent& event) override;

		friend class InputManager;
	};
}