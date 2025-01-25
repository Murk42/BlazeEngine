#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{		
	Vec2f NodeFinalTransform::TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition)
	{
		Vec2f localTransformPosition = (finalTransformPosition - position) / scale;

		if (rotation != 0)
		{
			float cos = Math::Cos(rotation);
			float sin = Math::Sin(rotation);
			Vec2f right = Vec2f(cos, sin);
			Vec2f up = Vec2f(-sin, cos);

			localTransformPosition = Vec2f(right.DotProduct(localTransformPosition), up.DotProduct(localTransformPosition));
		}

		return localTransformPosition;
	}

	Node::Node()
		: parent(nullptr), screen(nullptr), transform(), transformDirty(false), finalTransformDirty(false), enabled(true), parentEnabled(true)
	{				
	}	
	Node::~Node()
	{
		SetParent(nullptr);		
	}
	bool Node::HitTest(Vec2f screenPosition)		
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

		return Math::Shapes::Quad2Df({ Vec2f(), up, right + up, right }).IsInside(screenPosition - pos);
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

		if (oldParent != nullptr)		
			for (uintMem i = 0; i < oldParent->children.Count(); ++i)
				if (&oldParent->children[i] == this)
					oldParent->children.EraseAt(i);

		if (newParent != nullptr)
			newParent->children.AddBack(*this);

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
				.node = *newParent,
				.otherNode = this
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

			for (auto& child : children)
				child.MarkFinalTransformDirty();
		}
	}
	void Node::CleanTransform()
	{
		if (!transformDirty)
			return;

		transformDirty = false;

		transformUpdatedEventDispatcher.Call({ *this });		
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

		do CalculateFinalTransform();
		while (finalTransformDirty);
	}
	static Vec2f RotatePoint(Vec2f point, Vec2f around, float cos, float sin)
	{
		return Vec2f(
			cos * (point.x - around.x) - sin * (point.y - around.y),
			sin * (point.x - around.x) + cos * (point.y - around.y)
		) + around;
	}
	void Node::CalculateFinalTransform()
	{
		if (!finalTransformDirty)
			return;		

		float cos;
		float sin;		

		const NodeTransform transform = GetTransform();

		if (parent != nullptr)
		{			
			//Parent is required to be updated

			if (parent->finalTransformDirty)
				Debug::Logger::LogFatal("Blaze Engine Graphics", "Parent node transform is dirty while updating transform");

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
		}

		if (std::abs(cos) == 1 || std::abs(sin) == 1)
		{
			finalTransform.position.x = std::ceil(finalTransform.position.x);
			finalTransform.position.y = std::ceil(finalTransform.position.y);
		}
		
		finalTransformDirty = false;

		finalTransformUpdatedEventDispatcher.Call({
			.node = *this,
			.finalTransform = finalTransform
			});		
	}
	void Node::PropagateScreenChange(Screen* newScreen)
	{
		screen = newScreen;

		for (auto& child : children)
			child.PropagateScreenChange(newScreen);
	}	
	void Node::PropagateScreenChangeEvent(Screen* oldScreen)
	{
		surroundingNodeTreeChangedEventDispatcher.Call({
					.type = SurroundingNodeTreeChangedEvent::Type::ScreenChanged,
					.node = *this,
					.otherNode = oldScreen
			});

		for (auto& child : children)
			child.PropagateScreenChangeEvent(oldScreen);

	}	
	void Node::UpdateParentEnabledState()
	{
		bool newParentEnabled = enabled && (parent == nullptr ? true : parent->parentEnabled);

		if (parentEnabled != newParentEnabled)
		{
			parentEnabled = newParentEnabled;

			enabledStateUpdatedEventDispatcher.Call({ .node = *this });

			for (auto& child : children)
				child.PropagateEnabledState();
		}
	}

	void Node::PropagateEnabledState()
	{	
		bool newParentEnabled = parent->parentEnabled && enabled;

		if (newParentEnabled == parentEnabled)
			return;

		parentEnabled = newParentEnabled;

		enabledStateUpdatedEventDispatcher.Call({ *this });

		for (auto& child : children)			
			child.PropagateEnabledState();
	}

}