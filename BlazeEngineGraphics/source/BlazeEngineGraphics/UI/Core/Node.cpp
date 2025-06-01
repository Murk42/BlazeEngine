#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineCore/Math/Shapes.h"
#include "BlazeEngineCore/Debug/Logger.h"
#include "BlazeEngineGraphics/UI/Core/Screen.h"
#include "BlazeEngineGraphics/UI/Core/NodeTreeIterator.h"

namespace Blaze::UI
{	
	static Vec2f RotatePoint(Vec2f point, Vec2f around, float cos, float sin)
	{
		return Vec2f(
			cos * (point.x - around.x) - sin * (point.y - around.y),
			sin * (point.x - around.x) + cos * (point.y - around.y)
		) + around;
	}

	Node::Node()
		: parent(nullptr), screen(nullptr), next(nullptr), prev(nullptr), transformDirty(false), finalTransformDirty(false), enabled(true), parentEnabled(true), 
		insideTransformUpdatedEventHandler(false), insideFinalTransformUpdatedEventHandler(false)
	{				
	}	
	Node::~Node()
	{
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
		if (parent == newParent)
			return;

		Node* oldParent = parent;
		Screen* oldScreen = (oldParent == nullptr ? nullptr : oldParent->screen);
		Screen* newScreen = (newParent == nullptr ? nullptr : newParent->screen);
		
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
			Node* newPrevNode = GetNodeSubTreeTail(parent);
			
			prev = newPrevNode;			
			subTreeTail->next = newPrevNode->next;			

			prev->next = this;
			if (subTreeTail->next != nullptr)
				subTreeTail->next->prev = this;
			
			newParent->children.AddBack(*this);
		}		
		else
		{
			prev = nullptr;
			next = nullptr;			
		}

		MarkTransformDirty();
		MarkFinalTransformDirty();

		UpdateParentEnabledState();

		if (oldScreen != newScreen && oldScreen != nullptr)
			oldScreen->NodeRemoved(*this, oldParent);

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
				newScreen->NodeAdded(*this, oldParent);

			PropagateScreenChangeEvent(oldScreen);
		}
		else if (screen != nullptr)
			screen->NodeMoved(*this, oldParent);
	}
	void Node::SetTransform(const NodeTransform& transform)
	{		
		if (this->transform != transform)
		{			
			this->transform = transform;

			MarkTransformDirty();
			MarkFinalTransformDirty();
		}
	}
	void Node::MarkTransformDirty()
	{
		transformDirty = true;
	}	
	void Node::MarkFinalTransformDirty()
	{
		if (!finalTransformDirty)
		{
			finalTransformDirty = true;

			for (auto& child : GetChildren())
				child.MarkFinalTransformDirty();
		}
	}
	void Node::CleanTransform()
	{
		if (!transformDirty)
			return;
			

		if (!insideTransformUpdatedEventHandler)
		{
			insideTransformUpdatedEventHandler = true;
			transformUpdatedEventDispatcher.Call({ *this });
			insideTransformUpdatedEventHandler = false;
		}

		transformDirty = false;
	}
	void Node::CleanFinalTransform()
	{
		CleanTransform();
		CalculateFinalTransformUpwards();
	}
	void Node::Disable()
	{
		if (enabled == false)
			return;

		enabled = false;
		UpdateParentEnabledState();
	}
	void Node::Enable()
	{
		if (enabled == true)
			return;

		enabled = true;
		UpdateParentEnabledState();
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
		CalculateFinalTransformUpwards();
		return finalTransform;
	}		
	void Node::CalculateFinalTransformUpwards()
	{
		if (!finalTransformDirty)
			return;		

		if (parent != nullptr)						
			parent->CalculateFinalTransformUpwards();			

		CalculateFinalTransform();
	}	
	void Node::CalculateFinalTransform()
	{
		if (!finalTransformDirty)
			return;		

		float cos = 1.0f;
		float sin = 0.0f;

		const NodeTransform transform = GetTransform();

		if (parent != nullptr)
		{			
			//Parent is required to be updated

			if (parent->finalTransformDirty)
				BLAZE_ENGINE_GRAPHICS_FATAL("Parent node transform is dirty while updating transform");

			finalTransform.scale = parent->finalTransform.scale * transform.scale;
			finalTransform.size = transform.size * parent->finalTransform.scale;
			finalTransform.rotation = transform.rotation + parent->finalTransform.rotation;

			cos = Math::Cos(finalTransform.rotation);
			sin = Math::Sin(finalTransform.rotation);

			Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
			Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

			Vec2f parentRight = { Math::Cos(parent->finalTransform.rotation), Math::Sin(parent->finalTransform.rotation) };
			Vec2f parentUp = { -parentRight.y, parentRight.x };

			Vec2f offset = -finalTransform.size * transform.pivot;
			Vec2f parentRelativePos = 
				transform.parentPivot * parent->finalTransform.size + transform.pos * parent->finalTransform.scale +
				parentRelativeRight * offset.x + parentRelativeUp * offset.y;
								
			finalTransform.position =
				parent->finalTransform.position +
				parentRight * parentRelativePos.x + parentUp * parentRelativePos.y;			
		}
		else
		{			
			finalTransform.scale = transform.scale;
			finalTransform.size = transform.size * finalTransform.scale;
			finalTransform.rotation = transform.rotation;
			finalTransform.position = RotatePoint(transform.pos + transform.pivot * finalTransform.size, transform.pos, Math::Cos(finalTransform.rotation), Math::Sin(finalTransform.rotation));

			cos = Math::Cos(finalTransform.rotation);
			sin = Math::Sin(finalTransform.rotation);
		}

		if (std::abs(cos) == 1 || std::abs(sin) == 1)
		{
			finalTransform.position.x = std::ceil(finalTransform.position.x);
			finalTransform.position.y = std::ceil(finalTransform.position.y);
		}
		
		if (!insideFinalTransformUpdatedEventHandler)
		{
			insideFinalTransformUpdatedEventHandler = true;
			finalTransformUpdatedEventDispatcher.Call({
				.node = *this				
				});

			CalculateFinalTransform();
			insideFinalTransformUpdatedEventHandler = false;
		}

		finalTransformDirty = false;
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
	void Node::UpdateParentEnabledState()
	{
		bool newParentEnabled = enabled && (parent == nullptr ? true : parent->parentEnabled);

		if (parentEnabled != newParentEnabled)
		{
			parentEnabled = newParentEnabled;

			enabledStateChangedEventDispatcher.Call({ .node = *this });

			for (auto& child : GetChildren())
				child.PropagateEnabledState();
		}
	}
	void Node::PropagateEnabledState()
	{	
		bool newParentEnabled = parent->parentEnabled && enabled;

		if (newParentEnabled == parentEnabled)
			return;

		parentEnabled = newParentEnabled;

		enabledStateChangedEventDispatcher.Call({ *this });

		for (auto& child : GetChildren())
			child.PropagateEnabledState();
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