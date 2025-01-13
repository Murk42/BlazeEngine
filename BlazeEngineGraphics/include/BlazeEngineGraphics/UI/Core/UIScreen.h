#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UI
{
	struct NodeCreatedEvent
	{
		Node* node;
	};
	struct NodeDestroyedEvent
	{
		Node* node;
	};
	struct ScreenDestructionEvent
	{
		Screen* screen;
	};
	struct ScreenWindowChangedEvent
	{
		WindowBase* oldWindow;
		Screen* screen;
	};

	class Screen : 
		public Node,
		private EventHandler<Input::Events::WindowResizedEvent>
	{
	public:
		EventDispatcher<NodeCreatedEvent> nodeCreatedEventDispatcher;
		EventDispatcher<NodeDestroyedEvent> nodeDestroyedEventDispatcher;
		EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;
		EventDispatcher<ScreenWindowChangedEvent> screenWindowChangedEventDispatcher;

		Screen(WindowBase* window, bool resizeWithWindow = true);
		virtual ~Screen();
		
		void CalculateAllTransforms();
		
		void SetWindow(WindowBase* window, bool resizeWithWindow = true);

		virtual String GetTypeName() const { return "Screen"; };
		inline uintMem GetNodeCount() const { return nodeCount; }
		inline WindowBase* GetWindow() const { return window; }
	private:				
		uintMem nodeCount;
		WindowBase* window;

		void AddNode(Node* node);
		void RemoveNode(Node* node);

		void OnEvent(Input::Events::WindowResizedEvent event);

		friend class Node;
	};
}