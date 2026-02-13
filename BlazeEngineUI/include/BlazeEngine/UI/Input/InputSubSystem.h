#pragma once
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class PointerData;

	class BLAZE_API InputSubSystem
	{
	public:
		InputSubSystem();
		InputSubSystem(Screen& screen);
		~InputSubSystem();

		void SetScreen(Screen* screen);
		Screen* GetScreen() const;
		void SetWindow(Window* window);
		Window* GetWindow() const;

		void SelectNode(InputNode* node);
		inline InputNode* GetSelectedNode() const;
		
		Input::EventProcessedState ProcessInputEvent(const Input::GenericInputEvent& event, bool processed);

		bool SetMouseCapturingNode(Input::MouseID mouseID, InputNode* node);
		InputNode* GetMouseCapturingNode(Input::MouseID mouseID) const;
		Vec2f GetScreenMousePos(Input::MouseID mouseID) const;

		const Map<Input::MouseID, PointerData>& GetPointerMap() const;
	private:
		Screen* screen;
		Window* window;
		InputNode* selectedNode;
		Map<Input::MouseID, PointerData> pointerMap;

		void WindowDestructionEvent(const Window::DestructionEvent& event);
		void ScreenDestructionEvent(const Screen::DestructionEvent& event);
		void ScreenTreeChangedEvent(const Screen::ScreenTreeChangedEvent& event);
		void NodeEnabledStateChangedEvent(const Node::NodeEnabledStateChangedEvent& event);

		void NewScreenAndWindow();

		Input::EventProcessedState MouseEntersWindowEvent(const Input::MouseEntersWindowEvent& event, bool processed);
		Input::EventProcessedState MouseLeavesWindowEvent(const Input::MouseLeavesWindowEvent& event, bool processed);

		Input::EventProcessedState KeyDownEvent(const Input::KeyDownEvent& event, bool processed);
		Input::EventProcessedState KeyUpEvent(const Input::KeyUpEvent& event, bool processed);
		Input::EventProcessedState TextInputEvent(const Input::TextInputEvent& event, bool processed);
	};

	inline Screen* InputSubSystem::GetScreen() const { return screen; }
	inline Window* InputSubSystem::GetWindow() const { return window; }
	inline InputNode* InputSubSystem::GetSelectedNode() const { return selectedNode; }
	inline const Map<Input::MouseID, PointerData>& InputSubSystem::GetPointerMap() const { return pointerMap; }
}