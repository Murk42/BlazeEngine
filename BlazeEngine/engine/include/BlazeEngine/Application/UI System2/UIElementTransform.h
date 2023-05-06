#pragma once
#include "BlazeEngine/Application/ECS/Component.h"
#include "BlazeEngine/Application/ECS/Entity.h"
#include "BlazeEngine/Application/ECS/Scene.h"
#include "BlazeEngine/DataStructures/Rect.h"

namespace Blaze::UI2
{
	class UIScene;
	class UIElementTransformSystem;

	class BLAZE_API UIElementTransform : public ECS::Component
	{
		UIScene* scene;
		uint32 updateState;

		Vec2f viewportPos;
		Vec2f pos;
		Vec2f size;

		Align localAlignment;
		Align anchorAlignment;
		UIElementTransform* anchor;

		//bool active;
		//bool solid;
		//float depth;
		//UIElementTransform* clipElement;
		//Rectf clipRect;
	public:		
		UIElementTransform();

		inline UIScene* GetScene() const { return scene; }
		inline Vec2f GetViewportPos() const { return viewportPos; }

		void SetPos(Vec2f pos);
		Vec2f GetPos() const { return pos; }

		void SetSize(Vec2f size);
		Vec2f GetSize() const { return size; }

		void SetLocalAlignment(Align align);
		Align GetLocalAlignment() const { return localAlignment; }

		void SetAnchorAlignment(Align align);
		Align GetAnchorAlignment() const { return anchorAlignment; }

		void SetAnchor(UIElementTransform* anchor);
		UIElementTransform* GetAnchor() const { return anchor; }

		COMPONENT(UIElementTransform, UIElementTransformSystem);

		friend class System;
	};

	class BLAZE_API UIElementTransformSystem : public ECS::System
	{
		uint32 updateState;

		void ResolveClipRect(UIElementTransform* transfrom);
		void ResolveAlignedRect(UIElementTransform* transfrom);
		void CleanTransform(UIElementTransform* transfrom);
	public:
		void Update(const ECS::ComponentContainer& components);
	};
}