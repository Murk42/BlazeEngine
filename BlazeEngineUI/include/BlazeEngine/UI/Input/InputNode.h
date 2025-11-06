#pragma once
#include "BlazeEngine/Runtime/Input.h"
#include "BlazeEngine/Core/Event/EventHandler.h"
#include "BlazeEngine/UI/Core/Node.h"

namespace Blaze::UI
{
	class InputManager;

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
		struct MouseButtonDownEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::MouseID mouseID;

			Input::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
		};
		struct MouseButtonUpEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::MouseID mouseID;

			Input::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
		};
		struct MouseMotionEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::MouseID mouseID;

			Vec2f delta;  //In window pixel units
			Vec2f screenPos;
			Vec2f windowPos;
		};
		struct MouseScrollEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::MouseID mouseID;

			Vec2f value;
			Input::ScrollDirection direction;

			Vec2f screenPos;
			Vec2f windowPos;
		};
		struct MouseHitStatusChangedEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::MouseID mouseID;

			int oldHitStatus;
			int newHitStatus;
		};
		struct KeyDownEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::KeyboardID keyboardID;

			Input::Key key;
			Input::KeyModifier modifier;
			Input::KeyCode keyCode;

			bool repeat;
		};
		struct KeyUpEvent
		{
			InputNode& node;
			uint64 timeNS;
			Input::KeyboardID keyboardID;

			Input::Key key;
			Input::KeyModifier modifier;
			Input::KeyCode keyCode;
		};
		struct TextInputEvent
		{
			InputNode& node;
			uint64 timeNS;

			u8String string;
		};

		EventDispatcher<SelectedStateChangedEvent> selectedStateChangedEventDispatcher;
		EventDispatcher<MouseButtonDownEvent> mouseButtonDownEventDispatcher;
		EventDispatcher<MouseButtonUpEvent> mouseButtonUpEventDispatcher;
		EventDispatcher<MouseMotionEvent> mouseMotionEventDispatcher;
		EventDispatcher<MouseScrollEvent> mouseScrollEventDispatcher;
		EventDispatcher<MouseHitStatusChangedEvent> mouseHitStatusChangedEventDispatcher;
		EventDispatcher<KeyDownEvent> keyDownEventDispatcher;
		EventDispatcher<KeyUpEvent> keyUpEventDispatcher;
		EventDispatcher<TextInputEvent> textInputEventDispatcher;

		InputNode();
		~InputNode() override;

		/*
			\returns Value greater than 0 if the mouse is over the node and the node is blocking hits to the
			nodes bellow, less than 0 if the mouse is over the name and the node isn't blocking hits to the
			nodes bellow, and 0 if the mouse isn't over the node
		*/
		inline int GetMouseHitStatus() const { return mouseHitStatus; };
		inline MouseMotionReportPolicy GetMouseMotionReportPolicy() const { return mouseMotionReportPolicy; }
		bool IsSelected() const;
		inline bool IsSelectable() const { return selectable; }

		void SetMouseMotionReportPolicy(MouseMotionReportPolicy policy);
		void SetSelectableFlag(bool selectable);
		void Unselect();
		void Select();
	private:
		bool selectable : 1;
		int mouseHitStatus;
		InputManager* inputManager;
		MouseMotionReportPolicy mouseMotionReportPolicy;

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
		void EnabledStateChangedEvent(const Node::EnabledStateChangedEvent& event);
		void SurroundingNodeTreeChangedEvent(const Node::SurroundingNodeTreeChangedEvent& event);

		void SetMouseHitStatus(int value);

		friend class InputManager;
	};
}