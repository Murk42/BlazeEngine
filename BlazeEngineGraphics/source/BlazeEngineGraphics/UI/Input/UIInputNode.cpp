#include "pch.h"
#include "BlazeEngineGraphics/UI/Input/UIInputNode.h"

namespace Blaze::UI
{
	InputNode::InputNode()
		: Node(), keyboardHandler(nullptr), mouseHandler(nullptr), selectHandler(nullptr), blocksMouseEvents(false)
	{
	}
	InputNode::~InputNode()
	{
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
	void InputNode::SetBlocksMouseEventsFlag(bool blocksMouseEvents)
	{
		this->blocksMouseEvents = blocksMouseEvents;
	}	
}
