#include "pch.h"
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

namespace Blaze::UI
{
	static constexpr uint8 transformDirtyBit = 0;
	static constexpr uint8 destroyedBit = 0;	

	template<uint8 index>
	inline static bool GetValue(uint8 state)
	{
		return state & (index+ 1);
	}

	template<uint8 index>
	inline static void SetValue(uint8& state, bool value)
	{
		state = (state & (std::remove_reference_t<decltype(state)>)(~(index + 1))) | ((uint8)value << index);
	}

	Node::Node(Node* parent, const NodeTransform& transform)
		: parent(parent), screen(nullptr), transform(transform), state(0), renderStream(nullptr), finalRotation(0), transformState(0)
	{
		if (parent != nullptr)
		{
			screen = parent->screen;
			parent->children.AddBack(this);
			if (screen != nullptr)
				screen->AddNode(this);
		}
	}
	Node::~Node()
	{
		SetValue<destroyedBit>(state, true);

		if (parent != nullptr && !GetValue<destroyedBit>(parent->state))
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
	void Node::SetTransform(const NodeTransform& transform)
	{		
		this->transform = transform;

		MakeTransformDirty();
	}

	void Node::UpdateTransform()
	{
		MakeTransformDirty();		
	}

	void Node::ForceUpdateTransform()
	{
		CalculateTransformUpwards();
	}

	Vec2f Node::GetFinalPosition() const
	{
		CalculateTransformUpwards();		
		return finalPosition;
	}
	Vec2f Node::GetFinalSize() const
	{
		CalculateTransformUpwards();
		return finalSize;
	}
	float Node::GetFinalScale() const
	{		
		CalculateTransformUpwards();
		return finalScale;
	}
	float Node::GetFinalRotation() const
	{
		CalculateTransformUpwards();
		return finalRotation;
	}

	void Node::MakeTransformDirty() const
	{		
		if (!GetValue<transformDirtyBit>(state))
		{
			SetValue<transformDirtyBit>(state, true);

			for (auto& child : children)
				child->MakeTransformDirty();
		}
	}
	void Node::CalculateTransformUpwards() const
	{
		if (!GetValue<transformDirtyBit>(state))
			return;

		if (parent != nullptr)
			parent->CalculateTransformUpwards();

		CalculateTransform();
	}
	void Node::CalculateTransformDownwards() const
	{
		if (GetValue<transformDirtyBit>(state))
			CalculateTransform();			

		for (auto child : children)
			child->CalculateTransformDownwards();
	}
	Vec2f RotatePoint(Vec2f point, Vec2f around, float cos, float sin)
	{
		return Vec2f(
			cos * (point.x - around.x) - sin * (point.y - around.y),
			sin * (point.x - around.x) + cos * (point.y - around.y)
		) + around;
	}
	void Node::CalculateTransform() const
	{
		if (!GetValue<transformDirtyBit>(state))
			return;

		preTransformUpdateEventDispatcher.Call({ .node = (Node*)this });

		float cos;
		float sin;

		if (parent != nullptr)
		{
			//if (parent->transformDirty)
			//	parent->CalculateTransform();

			finalScale = parent->finalScale * transform.scale;
			finalSize = transform.size * finalScale;
			finalRotation = transform.rotation + parent->finalRotation;

			cos = Math::Cos(finalRotation);
			sin = Math::Sin(finalRotation);

			Vec2f parentRelativeRight = { Math::Cos(transform.rotation), Math::Sin(transform.rotation) };
			Vec2f parentRelativeUp = { -parentRelativeRight.y, parentRelativeRight.x };

			Vec2f parentRight = { Math::Cos(parent->finalRotation), Math::Sin(parent->finalRotation) };
			Vec2f parentUp = { -parentRight.y, parentRight.x };

			Vec2f offset = -finalSize * transform.pivot;
			Vec2f parentRelativePos = 
				transform.parentPivot * parent->finalSize + transform.pos * parent->finalScale +
				parentRelativeRight * offset.x + parentRelativeUp * offset.y;
								
			finalPosition =
				parent->finalPosition +
				parentRight * parentRelativePos.x + parentUp * parentRelativePos.y;			

		}
		else
		{			
			finalScale = transform.scale;
			finalSize = transform.size * finalScale;
			finalRotation = transform.rotation;

			cos = Math::Cos(finalRotation);
			sin = Math::Cos(finalRotation);

			Vec2f position = transform.pos;
			Vec2f offset = finalSize * transform.pivot;			

			finalPosition = RotatePoint(transform.pos + transform.pivot * finalSize, transform.pos, cos, sin);						
		}

		if (std::abs(cos) == 1 || std::abs(sin) == 1)
		{
			finalPosition.x = std::ceil(finalPosition.x);
			finalPosition.y = std::ceil(finalPosition.y);
		}

		++transformState;
		SetValue<transformDirtyBit>(state, false);		

		transformUpdatedEventDispatcher.Call({ .node = (Node*)this });
	}		
}