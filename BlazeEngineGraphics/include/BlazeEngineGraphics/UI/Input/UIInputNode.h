#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UI
{
	class UIKeyboardEventHandler;
	class UIMouseEventHandler;
	class UISelectEventHandler;

	class BLAZE_GRAPHICS_API InputNode : public Node
	{
	public:
		InputNode();
		~InputNode() override;

		void SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler);
		void SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler);
		void SetSelectEventHandler(UISelectEventHandler* selectEventHandler);

		void SetBlocksMouseEventsFlag(bool blocksMouseEvents);		
	private:		
		bool blocksMouseEvents;		

		UIKeyboardEventHandler* keyboardHandler;
		UIMouseEventHandler* mouseHandler;
		UISelectEventHandler* selectHandler;

		friend class InputManager;
	};
}