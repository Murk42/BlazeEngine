#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{
	Screen::Screen(WindowBase* window, bool resizeWithWindow)
		: Node(), window(nullptr), nodeCount(0)
	{
		SetTransform({
			.pos = Vec2f(0.0f, 0.0f),
			.parentPivot = Vec2f(0.0f, 0.0f),
			.pivot = Vec2f(0.0f, 0.0f),
			.size = Vec2f(0.0f, 0.0f),
			.scale = 1.0f,
			.rotation = 0.0f,
			});
		Node::screen = this;

		SetWindow(window, resizeWithWindow);
	}
	Screen::~Screen()
	{
		screenDestructionEventDispatcher.Call({ *this });
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

		screenWindowChangedEventDispatcher.Call({ .oldWindow = old, .screen = *this });
	}	
	void Screen::NodeAdded(Node& node, Node* oldParent)
	{
		nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeAdded, .node = node, .oldParent = oldParent });
		++nodeCount;
	}
	void Screen::NodeRemoved(Node& node, Node* oldParent)
	{
		--nodeCount;
		nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeRemoved, .node = node, .oldParent = oldParent });
	}
	void Screen::NodeMoved(Node& node, Node* oldParent)
	{
		nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeMoved, .node = node, .oldParent = oldParent });
	}
	void Screen::OnEvent(Input::Events::WindowResizedEvent event)
	{
		auto transform = GetTransform();
		transform.size = (Vec2f)event.size;
		SetTransform(transform);
	}
}