#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/InputManager.h"
#include "BlazeEngineGraphics/UI/Input/InputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), hit(0), inputManager(nullptr), keyboardHandler(nullptr), mouseHandler(nullptr), selectHandler(nullptr)
	{
		finalTransformUpdatedEventDispatcher.AddHandler(*this);
		enabledStateUpdatedEventDispatcher.AddHandler(*this);
	}
	InputNode::~InputNode()
	{		
		finalTransformUpdatedEventDispatcher.RemoveHandler(*this);
		enabledStateUpdatedEventDispatcher.RemoveHandler(*this);
	}
	void InputNode::SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler)
	{
		keyboardHandler = keyboardEventHandler;
	}
	void InputNode::SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler)
	{
		mouseHandler = mouseEventHandler;
	}
	void InputNode::SetSelectEventHandler(UISelectEventHandler* selectEventHandler)
	{
		selectHandler = selectEventHandler;
	}			
	void InputNode::OnEvent(const Node::FinalTransformUpdatedEvent& event)
	{		
		if (inputManager != nullptr && event.node.IsEnabled())
			inputManager->NodeFinalTransformUpdated(*this);
	}
	void InputNode::OnEvent(const Node::EnabledStateUpdatedEvent& event)
	{
		if (inputManager != nullptr)
		{
			auto screen = event.node.GetScreen();
			auto window = screen == nullptr ? nullptr : screen->GetWindow();

			inputManager->NodeEnabledStateChanged(*this);
		}
	}
}
