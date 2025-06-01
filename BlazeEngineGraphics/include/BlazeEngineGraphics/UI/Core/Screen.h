#pragma once
#include "BlazeEngine/Window/Window.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

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
		Window* oldWindow;
		Screen& screen;
	};

	class BLAZE_GRAPHICS_API Screen : 
		public Node,
		private EventHandler<Window::WindowResizedEvent>
	{
	public:
		EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;
		EventDispatcher<ScreenWindowChangedEvent> screenWindowChangedEventDispatcher;
		EventDispatcher<NodeTreeChangedEvent> nodeTreeChangedEventDispatcher;

		Screen(Window* window, bool resizeWithWindow = true);
		virtual ~Screen();

		void SetWindow(Window* window, bool resizeWithWindow = true);
		
		inline uintMem GetNodeCount() const { return nodeCount; }
		inline Window* GetWindow() const { return window; }
	private:						
		Window* window;
		uintMem nodeCount;

		void NodeAdded(Node& node, Node* oldParent);
		void NodeRemoved(Node& node, Node* oldParent);
		void NodeMoved(Node& node, Node* oldParent);
		
		void OnEvent(const Window::WindowResizedEvent& event) override;

		friend class Node;
	};
}