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

	class Screen : public Node
	{
	public:
		Blaze::EventDispatcher<NodeCreatedEvent> nodeCreatedEventDispatcher;
		Blaze::EventDispatcher<NodeDestroyedEvent> nodeDestroyedEventDispatcher;
		Blaze::EventDispatcher<ScreenDestructionEvent> screenDestructionEventDispatcher;

		Screen();
		virtual ~Screen();
		
		void CalculateAllTransforms();
		
		virtual String GetTypeName() const { return "Screen"; };
		inline uintMem GetNodeCount() const { return nodeCount; }
	private:		
		uintMem nodeCount;

		void AddNode(Node* node);
		void RemoveNode(Node* node);

		friend class Node;
	};
}