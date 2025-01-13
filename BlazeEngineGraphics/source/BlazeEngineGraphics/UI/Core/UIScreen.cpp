#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{
	Screen::Screen(WindowBase* window, bool resizeWithWindow)
		: Node(), nodeCount(0), window(nullptr)
	{
		Node::screen = this;

		SetWindow(window, resizeWithWindow);
	}
	Screen::~Screen()
	{
		screenDestructionEventDispatcher.Call({ this });
	}
	void Screen::CalculateAllTransforms()
	{
		CalculateFinalTransformDownwards();
	}
	void Screen::SetWindow(WindowBase* window, bool resizeWithWindow)
	{
		if (window == this->window)
			return;

		WindowBase* old = this->window;
		this->window = window;

		if (old != nullptr)
			UnsubscribeFromDispatcher();

		if (window != nullptr)
		{
			auto transform = GetTransform();
			transform.size = (Vec2f)window->GetSize();
			SetTransform(transform);

			if (resizeWithWindow)
				window->resizedEventDispatcher.AddHandler(*this);
		}

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
	void Screen::OnEvent(Input::Events::WindowResizedEvent event)
	{
		auto transform = GetTransform();
		transform.size = (Vec2f)event.size;
		SetTransform(transform);
	}
}