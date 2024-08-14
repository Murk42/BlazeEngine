#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{
	Screen::Screen()
		: Node(), nodeCount(0), window(nullptr)
	{
		Node::screen = this;
	}
	Screen::~Screen()
	{
		screenDestructionEventDispatcher.Call({ this });
	}
	void Screen::CalculateAllTransforms()
	{
		CalculateFinalTransformDownwards();
	}
	void Screen::SetWindow(WindowBase* window)
	{
		if (window == this->window)
			return;
		WindowBase* old = this->window;
		this->window = window;
		screenWindowChangedEventDispatcher.Call({ .oldWindow = old, .screen = this });
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