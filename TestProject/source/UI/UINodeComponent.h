#pragma once

namespace UI2
{
	struct NodeTransform
	{
		Vec2f pos;
		Vec2f parentPivot;
		Vec2f pivot;
		Vec2f size;
		Vec2f scale;
		float rotation;
	};

	class UINodeComponentSystem : public ECS::System
	{
	public:
		void Update(ECS::Component* component) override
		{

		}
	};

	class UINodeComponent : public ECS::Component
	{
	public:		
		UINodeComponent(const NodeTransform& transform, UINodeComponent* parent);
		UINodeComponent(const UINodeComponent&) = delete;
		~UINodeComponent();

		void SetTransform(const NodeTransform& transform);

		void CalculateTransformMatrix();
		
		void AddChild(UINodeComponent* component);

		inline NodeTransform GetTransform() const { return transform; }
		inline Mat4f GetTransformMatrix() const { return matrix; }
		inline Vec2f GetFinalSize() const { return finalSize; }
		inline UINodeComponent* GetParent() const { return parent; }

		UINodeComponent& operator=(const UINodeComponent&) = delete;

		COMPONENT(UINodeComponent, UINodeComponentSystem);
	private:	
		UINodeComponent* parent;
		UnorderedArray<UINodeComponent*> children;

		NodeTransform transform;
		bool dirty;
		Vec2f finalScale;
		Vec2f finalSize;
		Vec2f finalPosition;
		float finalRotation;

		Mat4f matrix;	
	};
}