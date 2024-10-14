#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UI
{
	class UIKeyboardEventHandler;
	class UIMouseEventHandler;
	class UISelectEventHandler;

	class BLAZE_GRAPHICS_API UIInputNode : public Node
	{
	public:
		UIInputNode();
		~UIInputNode() override;

		void SetKeyboardEventHandler(UIKeyboardEventHandler* keyboardEventHandler);
		void SetMouseEventHandler(UIMouseEventHandler* mouseEventHandler);
		void SetSelectEventHandler(UISelectEventHandler* selectEventHandler);

		void SetHitPropagation(bool hitPropagates);

		virtual bool HitTest(Vec2f screenPosition);
	private:
		bool hit;
		bool hitPropagates;

		UIKeyboardEventHandler* keyboardHandler;
		UIMouseEventHandler* mouseHandler;
		UISelectEventHandler* selectHandler;

		friend class UIInputManager;
	};
}