#include "pch.h"
#include "UINodeComponent.h"

namespace UI2
{
	UINodeComponent::UINodeComponent(const NodeTransform& transform, UINodeComponent* parent)
		: dirty(true), transform(transform), matrix(Mat4f::Identity()), parent(parent)
	{
	}
	UINodeComponent::~UINodeComponent()
	{
	}
	void UINodeComponent::SetTransform(const NodeTransform& transform)
	{
		dirty = true;
		this->transform = transform;
	}
	void UINodeComponent::CalculateTransformMatrix()
	{
		if (parent != nullptr)
		{
			if (parent->dirty)
				parent->CalculateTransformMatrix();

			finalScale = parent->finalScale * transform.scale;
			finalSize = transform.size * finalScale;

			Vec2f position = parent->finalSize * transform.parentPivot + transform.pos;
			Vec2f offset = finalSize * transform.pivot;

			matrix =
				parent->matrix *
				Mat4f::TranslationMatrix(Vec3f(position, 0)) *
				Mat4f::RotationMatrixAxisZ(transform.rotation) *
				Mat4f::TranslationMatrix(Vec3f(-offset, 0));
		}
		else
		{
			finalScale = transform.scale;
			finalSize = transform.size * finalScale;

			Vec2f position = transform.pos;
			Vec2f offset = finalSize * transform.pivot;

			matrix =
				Mat4f::TranslationMatrix(Vec3f(position, 0)) *
				Mat4f::RotationMatrixAxisZ(transform.rotation) *
				Mat4f::TranslationMatrix(Vec3f(-offset, 0));
		}

		dirty = false;

		for (auto& child : children)
			child->CalculateTransformMatrix();
	}
	void UINodeComponent::AddChild(UINodeComponent* component)
	{
		children.Add(component);
	}	
}