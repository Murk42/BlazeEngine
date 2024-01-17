#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{
	Screen::Screen()
		: Node(nullptr, { }), nodeCount(0)
	{
		Node::screen = this;
	}
	Screen::~Screen()
	{
		screenDestructionEventDispatcher.Call({ this });
	}
	void Screen::CalculateAllTransforms()
	{
		CalculateTransformDownwards();
	}
	void Screen::AddNode(Node* node)
	{		
		++nodeCount;
		nodeCreatedEventDispatcher.Call({ .node = node });
	}
	void Screen::RemoveNode(Node* node)
	{
		nodeDestroyedEventDispatcher.Call({ .node = node });
		--nodeCount;
	}
}