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

	class Screen : public Node
	{
	public:
		Blaze::EventDispatcher<NodeCreatedEvent> nodeCreatedEventDispatcher;
		Blaze::EventDispatcher<NodeDestroyedEvent> nodeDestroyedEventDispatcher;
		Blaze::EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;
		Blaze::EventDispatcher<ScreenWindowChangedEvent> screenWindowChangedEventDispatcher;

		Screen();
		virtual ~Screen();
		
		void CalculateAllTransforms();
		
		void SetWindow(WindowBase* window);

		virtual String GetTypeName() const { return "Screen"; };
		inline uintMem GetNodeCount() const { return nodeCount; }
		inline WindowBase* GetWindow() const { return window; }
	private:		
		uintMem nodeCount;
		WindowBase* window;

		void AddNode(Node* node);
		void RemoveNode(Node* node);

		friend class Node;
	};
}