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
		: parent(nullptr), screen(nullptr), transform(), transformDirty(false), finalTransformDirty(false), destroyed(false), transformState(0)
	{				
	}	
	Node::~Node()
	{
		destroyed = true;

		if (parent != nullptr && !parent->destroyed)
		{
			if (screen != nullptr)
				screen->RemoveNode(this);

			for (auto it = parent->children.FirstIterator(); it != parent->children.BehindIterator(); ++it)
				if (*it == this)
				{
					parent->children.EraseAt(it);
					break;
				}
		}
	}
	void Node::SetParent(Node* newParent)
	{		
		if (parent == newParent)
			return;

		if (parent != nullptr)
		{
			for (uintMem i = 0; i < parent->children.Count(); ++i)
				if (parent->children[i] == this)
					parent->children.EraseAt(i);

			if (parent->screen != nullptr && parent->screen != newParent->screen)
				screen->RemoveNode(this);
		}

		parent = newParent;

		if (parent != nullptr)
		{
			screen = parent->screen;

			parent->children.AddBack(this);

			if (screen != nullptr)
				screen->AddNode(this);
		}

		MarkTransformDirty();
		MarkFinalTransformDirty();
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
				child->MarkFinalTransformDirty();
		}
	}
	void Node::CleanTransform()
	{
		if (!transformDirty)
			return;

		transformDirty = false;

		transformUpdatedEventDispatcher.Call({ (UI::Node*)this });		
	}
	void Node::CleanFinalTransform()
	{
		CleanTransform();
		CalculateFinalTransformUpwards();
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
	void Node::CalculateFinalTransformDownwards()
	{
		while (finalTransformDirty)
			CalculateFinalTransform();

		for (auto child : children)
			child->CalculateFinalTransformDownwards();
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

			cos = Math::Cos(finalTransform.rotation);
			sin = Math::Cos(finalTransform.rotation);

			Vec2f position = transform.pos;
			Vec2f offset = finalTransform.size * transform.pivot;

			finalTransform.position = RotatePoint(transform.pos + transform.pivot * finalTransform.size, transform.pos, cos, sin);
		}

		if (std::abs(cos) == 1 || std::abs(sin) == 1)
		{
			finalTransform.position.x = std::ceil(finalTransform.position.x);
			finalTransform.position.y = std::ceil(finalTransform.position.y);
		}

		++transformState;
		finalTransformDirty = false;

		finalTransformUpdatedEventDispatcher.Call({
			.node = (Node*)this,
			.finalTransform = finalTransform
			});		
	}		
}