#pragma once
#include "BlazeEngine/Input/Mouse.h"
#include "BlazeEngine/Input/Keyboard.h"
#include "BlazeEngineCore/Event/EventHandler.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI
{
	class InputManager;

	class BLAZE_GRAPHICS_API InputNode : 
		public Node
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
			Mouse::MouseID mouseID;

			Mouse::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseButtonUpEvent
		{
			InputNode& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Mouse::MouseButton button;
			uint combo;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseMotionEvent
		{
			InputNode& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Vec2f delta;  //In window pixel units
			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseScrollEvent
		{
			InputNode& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			Vec2f value;
			Mouse::ScrollDirection direction;

			Vec2f screenPos;
			Vec2f windowPos;
			Vec2f desktopPos;
		};
		struct MouseHitStatusChangedEvent
		{
			InputNode& node;
			uint64 timeNS;
			Mouse::MouseID mouseID;

			int oldHitStatus;
			int newHitStatus;
		};
		struct KeyDownEvent
		{
			InputNode& node;
			uint64 timeNS;
			Keyboard::KeyboardID keyboardID;

			Keyboard::Key key;
			Keyboard::KeyModifier modifier;
			Keyboard::KeyCode keyCode;

			bool repeat;
		};
		struct KeyUpEvent
		{
			InputNode& node;
			uint64 timeNS;
			Keyboard::KeyboardID keyboardID;

			Keyboard::Key key;
			Keyboard::KeyModifier modifier;
			Keyboard::KeyCode keyCode;
		};
		struct TextInputEvent
		{
			InputNode& node;
			uint64 timeNS;

			StringUTF8 string;
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