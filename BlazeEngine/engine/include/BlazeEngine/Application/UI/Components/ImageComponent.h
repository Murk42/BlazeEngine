#pragma once
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/UISystem.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"

namespace Blaze::UI::Components
{
	class BLAZE_API ImageSystem : public UISystem
	{
	public:
		void Render(UIScene*, UIComponent*) override;

	};

	class BLAZE_API Image : public UIComponent
	{	 	
	public:
		Image(const Image&);
		Image(Vec2f uv1 = Vec2f(0), Vec2f uv2 = Vec2f(1), ColorRGBAf mask = 0xffffffff, float blend = 0.0f);
	
		void SetUVs(Vec2f uv1, Vec2f uv2);
		void SetMask(ColorRGBAf mask, float blend);
		
		inline ColorRGBAf GetMaskColor() const { return mask; }
		inline float GetBlendFactor() const { return blend; };
		inline Rectf GetUVRect() const { return { uv1, uv2 - uv1 }; }


		COMPONENT(Image, ImageSystem);		
	private:		
		Vec2f uv1;
		Vec2f uv2;
		ColorRGBAf mask;
		float blend;

		friend class System;
	};	
};