#pragma once
#include "BlazeEngine/Runtime/IO/Input.h"
#include "BlazeEngine/Runtime/IO/Window.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Input/InputNode.h"

namespace Blaze::UI
{
	class BLAZE_API PointerData
	{
	public:
		struct NodeHitData
		{
			Node& node;
			Node::HitStatus hitStatus;
		};

		PointerData(InputSubSystem& inputSubSystem, Vec2f pos);
		~PointerData();

		void Move(Vec2f newPos);

		void UpdateHitNodes();
		void SetCapturingNode(InputNode* node);

		void HandleNodeRemoval(Node& node);
		void HandleNodeAddition(Node& node);
		void HandleEnabledStateChange(Node& node);
		void HandleFinalTransformChange(Node& node);

		inline ArrayView<NodeHitData> GetHitData() const { return hitData; }
		inline Vec2f GetPos() const { return pos; }
		inline InputNode* GetCapturingNode() const { return capturingNode; }
	private:
		InputSubSystem& inputSubSystem;
		Array<NodeHitData> hitData;
		Vec2f pos;
		InputNode* capturingNode;
	};

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
		
		void InvalidateNodeHitStatus(InputNode& node);

		bool ProcessInputEvent(const Input::GenericInputEvent& event, bool eventProcessed);

		bool SetMouseCapturingNode(Input::MouseID mouseID, InputNode* node);
		InputNode* GetMouseCapturingNode(Input::MouseID mouseID) const;
		Vec2f GetScreenMousePos(Input::MouseID mouseID) const;

		const Map<Input::MouseID, PointerData>& GetPointerMap() const;
	private:
		Screen* screen;
		Window* window;
		InputNode* selectedNode;
		Map<Input::MouseID, PointerData> pointerMap;
		WindowMouseCaptureHandle mouseCaptureHandle;

		void TreeChangedEvent(const Screen::TreeChangedEvent& event);
		void ScreenDestructionEvent(const Screen::DestructionEvent& event);

		void WindowDestructionEvent(const Window::DestructionEvent& event);
		void WindowMouseEnterEvent(const Window::MouseEnterEvent& event);
		void WindowMouseLeaveEvent(const Window::MouseLeaveEvent& event);

		void NodeFinalTransformChangedEvent(const Node::FinalTransformUpdatedEvent& event);
		void EnabledStateChangedEvent(const Node::EnabledStateChangedEvent& event);

		bool MouseMotionEvent(const Input::MouseMotionEvent& event, bool eventProcessed);
		bool MouseScrollEvent(const Input::MouseScrollEvent& event, bool eventProcessed);
		bool MouseButtonDownEvent(const Input::MouseButtonDownEvent& event, bool eventProcessed);
		bool MouseButtonUpEvent(const Input::MouseButtonUpEvent& event, bool eventProcessed);
		bool KeyDownEvent(const Input::KeyDownEvent& event, bool eventProcessed);
		bool KeyUpEvent(const Input::KeyUpEvent& event, bool eventProcessed);
		bool TextInputEvent(const Input::TextInputEvent& event, bool eventProcessed);

		PointerData* FindOrInsertPointerData(Input::MouseID mouseID, Vec2f pos);
		const PointerData* FindPointerData(Input::MouseID mouseID) const;
		PointerData* FindPointerData(Input::MouseID mouseID);

		PointerData* PrepareMouseEvent(Input::MouseID mouseID, Vec2f pos, bool eventProcessed, bool insertMouse);
	};

	inline Screen* InputSubSystem::GetScreen() const { return screen; }
	inline Window* InputSubSystem::GetWindow() const { return window; }
	inline InputNode* InputSubSystem::GetSelectedNode() const { return selectedNode; }
	inline const Map<Input::MouseID, PointerData>& InputSubSystem::GetPointerMap() const { return pointerMap; }
}