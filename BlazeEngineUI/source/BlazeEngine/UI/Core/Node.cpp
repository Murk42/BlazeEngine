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
		finalTransformChanged(true), transformChanged(false), enabledStateChanged(false),
		deferChanges(false),
		containsDeferredEnabledFlag(false), containsDeferredTransform(false), containsDeferredParent(false),
		deferredEnabledFlag(false), deferredTransform(), deferredParent(nullptr)
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
		if (deferChanges)
			BLAZE_LOG_FATAL("Destroying a node inside a event callback. Which is not supported.");

		for (auto& child : children)
			child.SetParent(nullptr);

		SetParent(nullptr);

	}
	int Node::HitTest(Vec2f screenPosition)
	{
		auto finalTransform = GetFinalTransform();
		Vec2f pos = finalTransform.position;
		Vec2f size = finalTransform.size;
		float rot = finalTransform.rotation;
		float cos = Math::Cos(rot);
		float sin = Math::Sin(rot);
		Vec2f right = Vec2f(cos, sin) * size.x;
		Vec2f up = Vec2f(-sin, cos) * size.y;

		if (size.x == 0 || size.y == 0)
			return false;

		return (bool)Math::Shapes::Quad2Df({ Vec2f(), up, right + up, right }).IsInside(screenPosition - pos);
	}
	void Node::SetParent(Node* newParent)
	{
		if (deferChanges)
		{
			if (newParent == parent)
				containsDeferredParent = false;
			else
			{
				containsDeferredParent = true;
				deferredParent = newParent;
			}

			return;
		}

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

		StartDeferPeriod();

		if (oldScreen != newScreen && oldScreen != nullptr)
			oldScreen->nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeRemoved, .node = *this, .oldParent = oldParent });

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
				newScreen->nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeAdded, .node = *this, .oldParent = oldParent });

			PropagateScreenChangeEvent(oldScreen);
		}
		else if (screen != nullptr)
			screen->nodeTreeChangedEventDispatcher.Call({ .type = NodeTreeChangedEvent::Type::NodeMoved, .node = *this, .oldParent = oldParent });;

		PropagateEnabledStateEvent();

		EndDeferPeriod();
	}
	void Node::SetTransform(const NodeTransform& newTransform)
	{
		if (deferChanges)
		{
			if (newTransform == transform)
				containsDeferredTransform = false;
			else
			{
				containsDeferredTransform = true;
				deferredTransform = newTransform;
			}
		}

		if (transform != newTransform)
		{
			transform = newTransform;

			MarkTransformDirty();
			MarkFinalTransformDirty();
		}
	}
	void Node::MarkTransformDirty()
	{
		transformChanged = true;
		MarkFinalTransformDirty();
	}
	void Node::MarkFinalTransformDirty()
	{
		if (!finalTransformChanged)
		{
			finalTransformChanged = true;

			for (auto& child : GetChildren())
				child.MarkFinalTransformDirty();
		}
	}
	void Node::CleanTransform()
	{
		if (!transformChanged)
			return;

		transformChanged = false;

		StartDeferPeriod();
		transformUpdatedEventDispatcher.Call({ *this });
		EndDeferPeriod();
	}
	void Node::CleanFinalTransform()
	{
		if (!finalTransformChanged)
			return;

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

		CleanTransform();

		NodeFinalTransform newFinalTransform = CalculateFinalTransform(transform, parentFinalTransform);

		if (finalTransform != newFinalTransform)
		{
			finalTransformChanged = false;
			finalTransform = newFinalTransform;

			StartDeferPeriod();
			finalTransformUpdatedEventDispatcher.Call({ .node = *this });
			EndDeferPeriod();
		}
	}
	void Node::SetEnabledFlag(bool newEnabledFlag)
	{
		if (deferChanges)
		{
			if (newEnabledFlag == enabledFlag)
				containsDeferredEnabledFlag = false;
			else
			{
				deferredEnabledFlag = newEnabledFlag;
				containsDeferredEnabledFlag = true;
			}

			return;
		}

		enabledFlag = newEnabledFlag;

		PropagateEnabledState();
		PropagateEnabledStateEvent();
	}
	NodeTreeView Node::GetTree()
	{
		return NodeTreeView(this);
	}
	NodeTransform Node::GetTransform()
	{
		CleanTransform();
		return transform;
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

		if (std::abs(Math::Cos(newFinalTransform.rotation)) == 1 || std::abs(Math::Sin(newFinalTransform.rotation)) == 1)
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
		bool wasDeferPeriod = deferChanges;

		if (!wasDeferPeriod)
			StartDeferPeriod();

		surroundingNodeTreeChangedEventDispatcher.Call({
					.type = SurroundingNodeTreeChangedEvent::Type::ScreenChanged,
					.node = *this,
					.otherNode = oldScreen
			});

		for (auto& child : GetChildren())
			child.PropagateScreenChangeEvent(oldScreen);

		if (!wasDeferPeriod)
			EndDeferPeriod();
	}
	void Node::PropagateEnabledState()
	{
		bool newEnabled = enabledFlag && (parent == nullptr ? true : parent->enabled);

		if (enabled != newEnabled)
		{
			enabled = newEnabled;
			enabledStateChanged = true;

			for (auto& child : GetChildren())
				child.PropagateEnabledState();
		}
	}
	void Node::PropagateEnabledStateEvent()
	{
		if (enabledStateChanged)
		{
			enabledStateChanged = false;

			bool wasDeferPeriod = deferChanges;

			if (!wasDeferPeriod)
				StartDeferPeriod();

			enabledStateChangedEventDispatcher.Call({ .node = *this });

			for (auto& child : GetChildren())
				child.PropagateEnabledStateEvent();

			if (!wasDeferPeriod)
				EndDeferPeriod();
		}
	}
	void Node::StartDeferPeriod()
	{
		deferChanges = true;
	}
	void Node::EndDeferPeriod()
	{
		deferChanges = false;

		if (containsDeferredParent)
		{
			containsDeferredParent = false;
			SetParent(deferredParent);
		}
		if (containsDeferredTransform)
		{
			containsDeferredTransform = false;
			SetTransform(deferredTransform);
		}
		if (containsDeferredEnabledFlag)
		{
			containsDeferredEnabledFlag = false;
			SetEnabledFlag(deferredEnabledFlag);
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