#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UI
{
	struct NodeTreeChangedEvent
	{
		enum class Type
		{
			NodeAdded,
			NodeRemoved,
			NodeMoved
		};

		Type type;
		Node& node;		
		Node* oldParent;
	};	
	struct ScreenDestructionEvent
	{
		Screen& screen;
	};
	struct ScreenWindowChangedEvent
	{
		WindowBase* oldWindow;
		Screen& screen;
	};

	class Screen : 
		public Node,
		private EventHandler<Input::Events::WindowResizedEvent>
	{
	public:
		EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;
		EventDispatcher<ScreenWindowChangedEvent> screenWindowChangedEventDispatcher;
		EventDispatcher<NodeTreeChangedEvent> nodeTreeChangedEventDispatcher;

		Screen(WindowBase* window, bool resizeWithWindow = true);
		virtual ~Screen();

		void SetWindow(WindowBase* window, bool resizeWithWindow = true);
		
		inline uintMem GetNodeCount() const { return nodeCount; }
		inline WindowBase* GetWindow() const { return window; }
	private:						
		WindowBase* window;
		uintMem nodeCount;

		void NodeAdded(Node& node, Node* oldParent);
		void NodeRemoved(Node& node, Node* oldParent);
		void NodeMoved(Node& node, Node* oldParent);
		
		void OnEvent(Input::Events::WindowResizedEvent event);

		friend class Node;
	};
}