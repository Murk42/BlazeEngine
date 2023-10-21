#include "pch.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Application/UI/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI/Components/Bases/UIPosOverrideComponent.h"
#include "BlazeEngine/Application/UI/Components/Bases/UISizeOverrideComponent.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze::UI::Components
{	
	UITransformComponent::UITransformComponent()
		: updateState(0), posOverrideComponent(nullptr), sizeOverrideComponent(nullptr), finalSize(0, 0), finalPos(0, 0),
		pos(0, 0), size(100, 100), scale(1), rotation(0),		
		pivot(0.5, 0.5), anchorPivot(0.5, 0.5), anchor(nullptr)		
	{				
		elementComponent = GetEntity()->GetComponent<UIElementComponent>();

		for (uint i = 2; i < GetEntity()->GetComponentCount(); ++i)
		{
			auto& tags = GetEntity()->GetComponentTypeData(i)->GetTypeTags();			

			if (tags.Contains("UIPosOverrideComponent"))			
				posOverrideComponent = (UIPosOverrideComponent*)GetEntity()->GetComponent(i);
			if (tags.Contains("UISizeOverrideComponent"))
				sizeOverrideComponent = (UISizeOverrideComponent*)GetEntity()->GetComponent(i);						
		}
	}

	UITransformComponent::UITransformComponent(const UITransformComponent& other)
		: updateState(0), posOverrideComponent(nullptr), sizeOverrideComponent(nullptr), finalSize(0, 0), finalPos(0, 0),
		pos(other.pos), size(other.size), scale(other.scale), rotation(other.rotation),		
		pivot(other.pivot), anchorPivot(other.anchorPivot), anchor(nullptr)
	{				
		SetAnchor(other.anchor);
	}	

	UITransformComponent::UITransformComponent(const UITransformData& other)
		: updateState(0), posOverrideComponent(nullptr), sizeOverrideComponent(nullptr), finalSize(0, 0), finalPos(0, 0),
		pos(other.pos), size(other.size), scale(other.scale), rotation(other.rotation),		
		pivot(other.pivot), anchorPivot(other.anchorPivot), anchor(nullptr)
	{	
		SetAnchor(other.anchor);
	}

	UITransformComponent::~UITransformComponent()
	{
		for (auto child : children)
		{
			child->anchor = nullptr;
			child->childIterator = DualList<UITransformComponent*>::Iterator();
		}		
	}
	Vec2f UITransformComponent::ToLocalCoordinates(Vec2f point) const
	{
		point /= finalScale;
		
		float c = Math::Cos(-finalRotation);
		float s = Math::Sin(-finalRotation);

		point = Vec2f(c * point.x - s * point.y, s * point.x + c * point.y);

		point -= finalPos;		

		return point;
	}


	void UITransformComponent::SetAnchor(UITransformComponent* anchor)
	{
		if (this->anchor == anchor)
			return;

		if (this->anchor != nullptr)		
			this->anchor->children.Erase(childIterator);		

		this->anchor = anchor;

		if (anchor != nullptr)
		{
			anchor->children.AddFront(this);
			childIterator = anchor->children.FirstIterator();
		}		
	}	

	void UITransformComponent::Setup(const UITransformData& other)
	{
		this->pos = other.pos;
		this->size = other.size;
		this->scale = other.scale;
		this->rotation = other.rotation;
		this->pivot = other.pivot;
		this->anchorPivot = other.anchorPivot;
		SetAnchor(other.anchor);		
	}

	UITransformComponent& UITransformComponent::operator=(const UITransformComponent& other)
	{
		this->pos = other.pos;
		this->size = other.size;
		this->scale = other.scale;
		this->rotation = other.rotation;
		this->pivot = other.pivot;
		this->anchorPivot = other.anchorPivot;
		SetAnchor(other.anchor);		
		return *this;
	}
}