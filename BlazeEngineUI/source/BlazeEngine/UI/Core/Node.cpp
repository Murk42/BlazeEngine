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
		transformDirty(false), finalTransformDirty(true), isFilteringTransform(false)
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

		MarkTransformDirty();

		PropagateEnabledState();

		if (oldScreen != newScreen && oldScreen != nullptr)
			oldScreen->treeChangedEventDispatcher.Call({ .type = Screen::TreeChangedEvent::Type::NodeRemoved, .node = *this, .oldParent = oldParent });

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
				newScreen->treeChangedEventDispatcher.Call({ .type = Screen::TreeChangedEvent::Type::NodeAdded, .node = *this, .oldParent = oldParent });

			PropagateScreenChangeEvent(oldScreen);
		}
		else if (screen != nullptr)
			screen->treeChangedEventDispatcher.Call({ .type = Screen::TreeChangedEvent::Type::NodeMoved, .node = *this, .oldParent = oldParent });;

		PropagateEnabledStateEvent();
	}
	void Node::SetTransform(const NodeTransform& newTransform)
	{
		if (isFilteringTransform)
			transform = newTransform;
		else
		{
			bool wasTransformDirty = transformDirty;
			transformDirty = false;

			NodeTransform oldTransform = transform;
			transform = newTransform;

			isFilteringTransform = true;
			transformFilterEventDispatcher.Call({ *this, transform });
			isFilteringTransform = false;

			if (oldTransform != transform)
			{
				MarkFinalTransformDirtyDownwards();

				if (!wasTransformDirty)
					transformUpdatedEventDispatcher.Call({ *this });
			}
		}
	}
	void Node::MarkTransformDirty()
	{
		if (!isFilteringTransform)
		{
			bool wasTransformDirty = transformDirty;
			transformDirty = true;
			finalTransformDirty = true;

			if (!wasTransformDirty)
				transformUpdatedEventDispatcher.Call({ *this });
		}
	}
	void Node::CleanTransform()
	{
		if (transformDirty)
			SetTransform(transform);
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
	bool Node::GetTransform(NodeTransform& transform)
	{
		bool wasDirty = transformDirty && !isFilteringTransform;
		transform = GetTransform();
		return wasDirty;
	}
	NodeTransform Node::GetTransform()
	{
		if (isFilteringTransform)
			return transform;

		CleanTransform();

		return transform;
	}
	bool Node::GetFinalTransform(NodeFinalTransform& transform)
	{
		bool wasDirty = finalTransformDirty;
		transform = GetFinalTransform();
		return wasDirty;
	}
	NodeFinalTransform Node::GetFinalTransform()
	{
		CleanFinalTransform();

		return finalTransform;
	}
	NodeFinalTransform Node::CalculateFinalTransform(const NodeTransform& transform, const NodeFinalTransform& parentFinalTransform)
	{
		NodeFinalTransform newFinalTransform;

		newFinalTransform.scale = parentFinalTransform.scale * transform.scale;
		newFinalTransform.size = transform.size * parentFinalTransform.scale;
		newFinalTransform.rotation = transform.rotation + parentFinalTransform.rotation;

		Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
		Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

		Vec2f parentRight = { Math::Cos(parentFinalTransform.rotation), Math::Sin(parentFinalTransform.rotation) };
		Vec2f parentUp = { -parentRight.y, parentRight.x };

		Vec2f offset = -newFinalTransform.size * transform.pivot;
		Vec2f parentRelativePos =
			transform.parentPivot * parentFinalTransform.size + transform.pos * parentFinalTransform.scale +
			parentRelativeRight * offset.x + parentRelativeUp * offset.y;

		newFinalTransform.position =
			parentFinalTransform.position +
			parentRight * parentRelativePos.x + parentUp * parentRelativePos.y;

		newFinalTransform.right = { Math::Cos(newFinalTransform.rotation), Math::Sin(newFinalTransform.rotation) };

		if (std::abs(newFinalTransform.right.x) == 1 || std::abs(newFinalTransform.right.y) == 1)
		{
			newFinalTransform.position.x = Math::Ceil(newFinalTransform.position.x);
			newFinalTransform.position.y = Math::Ceil(newFinalTransform.position.y);
		}

		return newFinalTransform;
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
	void Node::CleanFinalTransform()
	{
		if (!finalTransformDirty)
			return;

		CleanTransform();

		finalTransformDirty = false;

		NodeFinalTransform parentFinalTransform;

		if (parent != nullptr)
		{
			parent->CleanFinalTransform();
			parentFinalTransform = parent->finalTransform;
		}
		else
		{
			parentFinalTransform = NodeFinalTransform{ .position = {}, .size = {}, .scale = 1.0f, .rotation = 0.0f };
		}

		NodeFinalTransform newFinalTransform = CalculateFinalTransform(transform, parentFinalTransform);

		if (finalTransform != newFinalTransform)
		{
			finalTransform = newFinalTransform;
			finalTransformUpdatedEventDispatcher.Call({ .node = *this });
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