#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Math/Vector.h"
#include "BlazeEngine/Runtime/IO/InputTypes.h"
#include "BlazeEngine/Runtime/IO/WindowMouseCaptureHandle.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/UI/Input/InputNode.h"
#include "BlazeEngine/UI/Input/InputEvents.h"

namespace Blaze::UI
{
	class InputSubSystem;

	class BLAZE_API PointerData
	{
	public:
		struct NodeHitData
		{
			Node& node;
			Node::HitStatus hitStatus;
		};

		PointerData(Screen& screen, Window& window, Input::MouseID mouseID, Vec2f windowPos, bool blocked);
		~PointerData();

		bool SetCapturingNode(InputNode* node);

		Input::EventProcessedState MouseMotionEvent(const Input::MouseMotionEvent& event, bool processed);
		Input::EventProcessedState MouseScrollEvent(const Input::MouseScrollEvent& event, bool processed);
		Input::EventProcessedState MouseButtonDownEvent(const Input::MouseButtonDownEvent& event, bool processed);
		Input::EventProcessedState MouseButtonUpEvent(const Input::MouseButtonUpEvent& event, bool processed);

		inline ArrayView<NodeHitData> GetHitData() const { return hitData; }
		inline Vec2f GetPos() const { return pos; }
		inline InputNode* GetCapturingNode() const { return capturingNode; }
	private:
		Screen& screen;
		Window& window;
		Input::MouseID mouseID;
		Vec2f pos;
		Vec2f windowPos;
		bool blocked;
		bool syntheticMouseMoveEventSent;

		Array<NodeHitData> hitData;

		InputNode* capturingNode;
		WindowMouseCaptureHandle mouseCaptureHandle;		

		void ScreenTreeChangedEvent(const Screen::ScreenTreeChangedEvent& event);
		void NodeFinalTransformChangedEvent(const Node::FinalTransformUpdatedEvent& event);
		void NodeEnabledStateChangedEvent(const Node::NodeEnabledStateChangedEvent& event);
		void NodeHitTestInvalidatedEvent(const Node::HitTestInvalidatedEvent& event);
		void SubscribeToNode(Node& node);
		void UnsubscribeFromNode(Node& node);

		void CapturedMouseEvent(const WindowMouseCaptureHandle::CapturedMouseEvent& event);

		void UpdateSceneMouseCollision();
		void UpdateNodeHitStatus(Node& node, bool useNotHitAsHitStatus = false);
		void UpdateHitNodes();

		template<typename F> requires std::invocable<F, InputNode&>
		bool DispatchEvent(F function);
	};
}