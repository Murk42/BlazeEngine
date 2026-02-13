#include "pch.h"
#include "BlazeEngine/UI/Core/Node.h"
#include "BlazeEngine/UI/Core/Screen.h"
#include "BlazeEngine/Core/Math/Shapes.h"
#include "BlazeEngine/Core/Math/Math.h"

namespace Blaze::UI
{
	static Vec2f RotatePoint(Vec2f point, Vec2f around, float cos, float sin)
	{
		return Vec2f(
			cos * (point.x - around.x) - sin * (point.y - around.y),
			sin * (point.x - around.x) + cos * (point.y - around.y)
		) + around;
	}

	Node::Node() :
		screen(nullptr), parent(nullptr), next(nullptr), prev(nullptr), children(),
		transform(), finalTransform(),
		enabled(true), enabledFlag(true),
		transformDirty(false), finalTransformDirty(true)
	{
	}
	Node::Node(Node& parent, const NodeTransform& transform)
		: Node()
	{
		SetTransform(transform);
		SetParent(&parent);
	}
	Node::~Node()
	{
		for (auto& child : children)
			child.SetParent(nullptr);

		SetParent(nullptr);

	}
	Node::HitStatus Node::HitTest(Vec2f screenNodePosition)
	{
		auto finalTransform = GetFinalTransform();
		Vec2f pos = finalTransform.position;
		Vec2f size = finalTransform.size;

		if (size.x == 0 || size.y == 0)
			return HitStatus::NotHit;

		float rot = finalTransform.rotation;
		float cos = Math::Cos(rot);
		float sin = Math::Sin(rot);
		Vec2f right = Vec2f(cos, sin) * size.x;
		Vec2f up = Vec2f(-sin, cos) * size.y;

		if (Math::Shapes::Quad2Df({ Vec2f(), up, right + up, right }).IsInside(screenNodePosition - pos))
			return HitStatus::HitBlocking;

		return HitStatus::NotHit;
	}
	void Node::InvalidateHitTest()
	{
		hitTestInvalidatedEventDispatcher.Call({ *this });
	}
	void Node::SetParent(Node* newParent)
	{
		if (parent == newParent)
			return;

		Node* oldParent = parent;
		Screen* oldScreen = screen;
		Screen* newScreen = (newParent == nullptr ? nullptr : (newParent->screen == nullptr ? dynamic_cast<Screen*>(newParent) : newParent->screen));

		if (oldScreen != newScreen)
			PropagateScreenChange(newScreen);

		parent = newParent;

		Node* subTreeTail = GetNodeSubTreeTail(this);

		if (oldParent != nullptr)
		{
			for (uintMem i = 0; i < oldParent->children.Count(); ++i)
				if (&oldParent->children[i] == this)
					oldParent->children.EraseAt(i);

			if (prev != nullptr)
				prev->next = subTreeTail->next;
			if (subTreeTail->next != nullptr)
				subTreeTail->next->prev = prev;
		}

		if (newParent != nullptr)
		{
			Node* newPrevNode = GetNodeSubTreeTail(newParent);
			Node* newNextNode = newPrevNode->next;

			prev = newPrevNode;
			subTreeTail->next = newNextNode;

			newPrevNode->next = this;
			if (newNextNode != nullptr)
				newNextNode->prev = subTreeTail;

			newParent->children.AddBack(*this);
		}
		else
		{
			prev = nullptr;
			subTreeTail->next = nullptr;
		}

		MarkFinalTransformDirty();

		PropagateEnabledState();

		if (oldScreen != newScreen && oldScreen != nullptr)
			oldScreen->treeChangedEventDispatcher.Call({ .type = Screen::ScreenTreeChangedEvent::Type::NodeRemoved, .node = *this, .oldParent = oldParent });

		if (oldParent != nullptr)
			oldParent->surroundingNodeTreeChangedEventDispatcher.Call({
				.type = SurroundingNodeTreeChangedEvent::Type::ChildRemoved,
				.node = *oldParent,
				.otherNode = this
				});

		surroundingNodeTreeChangedEventDispatcher.Call({
				.type = SurroundingNodeTreeChangedEvent::Type::ParentChanged,
				.node = *this,
				.otherNode = oldParent
			});

		if (newParent != nullptr)
			newParent->surroundingNodeTreeChangedEventDispatcher.Call({
				.type = SurroundingNodeTreeChangedEvent::Type::ChildAdded,
				.node = *this,
				.otherNode = newParent
				});

		if (oldScreen != newScreen)
		{
			if (newScreen != nullptr)
				newScreen->treeChangedEventDispatcher.Call({ .type = Screen::ScreenTreeChangedEvent::Type::NodeAdded, .node = *this, .oldParent = oldParent });

			PropagateScreenChangeEvent(oldScreen);
		}
		else if (screen != nullptr)
			screen->treeChangedEventDispatcher.Call({ .type = Screen::ScreenTreeChangedEvent::Type::NodeMoved, .node = *this, .oldParent = oldParent });;

		PropagateEnabledStateEvent();
	}
	void Node::SetTransform(const NodeTransform& newTransform)
	{
		transform = newTransform;
		transformDirty = true;
	}
	void Node::SetFinalTransform(const NodeFinalTransform& newFinalTransform)
	{
		finalTransform = newFinalTransform;
		finalTransformDirty = true;
	}
	void Node::MarkTransformDirty()
	{
		transformDirty = true;
	}
	void Node::MarkFinalTransformDirty()
	{
		finalTransformDirty = true;
	}
	void Node::SetEnabledFlag(bool newEnabledFlag)
	{
		enabledFlag = newEnabledFlag;

		PropagateEnabledState();
		PropagateEnabledStateEvent();
	}
	NodeTreeView Node::GetTree()
	{
		return NodeTreeView(this);
	}
	void Node::PropagateScreenChange(Screen* newScreen)
	{
		screen = newScreen;

		for (auto& child : GetChildren())
			child.PropagateScreenChange(newScreen);
	}
	void Node::PropagateScreenChangeEvent(Screen* oldScreen)
	{
		surroundingNodeTreeChangedEventDispatcher.Call({
					.type = SurroundingNodeTreeChangedEvent::Type::ScreenChanged,
					.node = *this,
					.otherNode = oldScreen
			});

		for (auto& child : GetChildren())
			child.PropagateScreenChangeEvent(oldScreen);
	}
	void Node::PropagateEnabledState()
	{
		bool newEnabled = enabledFlag && (parent == nullptr ? true : parent->enabled);

		if (enabled != newEnabled)
		{
			enabled = newEnabled;

			for (auto& child : GetChildren())
				child.PropagateEnabledState();
		}
	}
	void Node::PropagateEnabledStateEvent()
	{
		enabledStateChangedEventDispatcher.Call({ .node = *this });

		for (auto& child : GetChildren())
			child.PropagateEnabledStateEvent();
	}
	void Node::MarkFinalTransformDirtyDownwards()
	{
		if (!finalTransformDirty)
		{
			finalTransformDirty = true;

			for (auto& child : GetChildren())
				child.MarkFinalTransformDirtyDownwards();
		}
	}
	bool Node::AreNodesOnSameLevel(Node* a, Node* b)
	{
		while (a != nullptr && b != nullptr)
		{
			a = a->parent;
			b = b->parent;
		}

		if (a == nullptr && b == nullptr)
			return true;

		return false;
	}
	Node* Node::GetNodeSubTreeTail(Node* node)
	{
		Node* subTreeTail = node;
		while (!subTreeTail->children.Empty())
			subTreeTail = &subTreeTail->children.Last();

		return subTreeTail;
	}
}