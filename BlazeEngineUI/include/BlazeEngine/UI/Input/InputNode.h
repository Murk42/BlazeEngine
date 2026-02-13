#pragma once
#include "BlazeEngine/Core/Event/EventHandler.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Input/InputEvents.h"

namespace Blaze::UI
{
	class InputSubSystem;

	class BLAZE_API InputNode : public Node
	{
	public:
		enum class MouseMotionReportPolicy : uint8
		{
			ReportWhileMouseInside,
			ReportWhileSelected
		};

		struct SelectedStateChangedEvent
		{
			InputNode& node;
		};

		EventDispatcher<SelectedStateChangedEvent>    selectedStateChangedEventDispatcher;
		EventDispatcher<UIMouseButtonDownEvent>       mouseButtonDownEventDispatcher;
		EventDispatcher<UIMouseButtonUpEvent>         mouseButtonUpEventDispatcher;
		EventDispatcher<UIMouseMotionEvent>    	      mouseMotionEventDispatcher;
		EventDispatcher<UIMouseHitStatusChangedEvent> mouseHitStatusChangedEventDispatcher;
		EventDispatcher<UIMouseScrollEvent>    	      mouseScrollEventDispatcher;
		EventDispatcher<UIKeyDownEvent>    		      keyDownEventDispatcher;
		EventDispatcher<UIKeyUpEvent>    		      keyUpEventDispatcher;
		EventDispatcher<UITextInputEvent>    	      textInputEventDispatcher;

		InputNode();
		~InputNode() override;

		InputSubSystem* GetInputSubSystem() const;

		void SetSelectableFlag(bool selectable);
		inline bool IsSelectable() const { return selectable; }

		void Unselect();
		void Select();
		bool IsSelected() const;
		
		bool CaptureMouse(Input::MouseID mouseID);
		bool ReleaseMouse(Input::MouseID mouseID);
	private:
		InputSubSystem* inputSubSystem;
		bool selectable : 1;

		friend class InputSubSystem;
	};

	inline InputSubSystem* InputNode::GetInputSubSystem() const { return inputSubSystem; }
}