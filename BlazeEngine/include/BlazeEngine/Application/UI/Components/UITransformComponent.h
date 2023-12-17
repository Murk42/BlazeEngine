#pragma once
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Event/EventDispatcher.h"

namespace Blaze::UI
{
	class UIScene;
}

namespace Blaze::UI::Components
{		
	class UITransformComponent;

	class UIElementComponent;
	class UISizeOverrideComponent;
	class UIPosOverrideComponent;

	struct UITransformData
	{
		Vec2f pos;
		Vec2f size;

		float scale;
		float rotation;

		Vec2f pivot;
		Vec2f anchorPivot;
		UITransformComponent* anchor;		

		UITransformData(Vec2f pos = Vec2f(0), Vec2f size = Vec2f(100), float scale = 1.0f, float rotation = 0.0f, Vec2f pivot = Vec2f(0.5f), Vec2f anchorPivot = Vec2f(0.5f), UITransformComponent* anchor = nullptr)
			: pos(pos), size(size), scale(scale), rotation(rotation), pivot(pivot), anchorPivot(anchorPivot), anchor(anchor)
		{

		}

	};

	class BLAZE_API UITransformComponent : public ECS::Component
	{		
	public:		
		Vec2f pos;
		Vec2f size;

		float scale;
		float rotation;

		Vec2f pivot;
		Vec2f anchorPivot;
		
		UITransformComponent();
		UITransformComponent(const UITransformComponent&);		
		UITransformComponent(const UITransformData& data);
		~UITransformComponent();

		//Lower left corner of the transform becomes (0, 0) and gets scaled back to original size
		Vec2f ToLocalCoordinates(Vec2f point) const;
				
		inline Mat4f GetMatrix() const { return matrix; }
		inline Vec2f GetFinalSize() const { return finalSize; }		
		inline Vec2f GetFinalPos() const { return finalPos; }
		inline float GetFinalRotation() const { return finalRotation; }
		inline float GetFinalScale() const { return finalScale; }

		inline const DualList<UITransformComponent*>& GetChildren() const { return children; }

		void SetAnchor(UITransformComponent* anchor);
		UITransformComponent* GetAnchor() const { return anchor; }						

		void Setup(const UITransformData& data);

		UITransformComponent& operator=(const UITransformComponent&);

		COMPONENT(UITransformComponent, ECS::System);

		friend class UIScene;		
	private:		
		UIElementComponent* elementComponent;
		UIPosOverrideComponent* posOverrideComponent;
		UISizeOverrideComponent* sizeOverrideComponent;		

		uint32 updateState;
		
		Mat4f matrix;
		float finalScale;
		float finalRotation;
		Vec2f finalSize;
		Vec2f finalPos;
		
		UITransformComponent* anchor;						
		
		DualList<UITransformComponent*> children;
		DualList<UITransformComponent*>::Iterator childIterator;
	};
}