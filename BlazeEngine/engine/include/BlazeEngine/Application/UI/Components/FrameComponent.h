#pragma once
#include "BlazeEngine/Application/UI/UIComponent.h"
#include "BlazeEngine/Application/UI/UISystem.h"
#include "BlazeEngine/DataStructures/Color.h"

namespace Blaze::UI::Components
{
	class BLAZE_API FrameSystem : public UISystem
	{
	public:
		void Render(UIScene*, UIComponent*) override;
	};
	class BLAZE_API Frame : public UIComponent
	{		
	public:		
		Frame(const Frame&);		
		Frame(ColorRGBAf fillColor = 0x222222ff, ColorRGBAf borderColor = 0x333333ff, float borderWidth = 2, float cornerRadius = 8);

		void Setup(ColorRGBAf fillColor = 0x222222ff, ColorRGBAf borderColor = 0x333333ff, float borderWidth = 2, float cornerRadius = 8);

		void SetBorderWidth(float borderWidth);
		void SetCornerRadius(float cornerRadius);
		void SetFillColor(ColorRGBAf fillColor);
		void SetBorderColor(ColorRGBAf borderColor);

		inline float GetBorderWidth() const { return borderWidth; }
		inline float GetCornerRadius() const { return cornerRadius; }
		inline ColorRGBAf GetFillColor() const { return fillColor; }
		inline ColorRGBAf GetBorderColor() const { return borderColor; }			
		
		COMPONENT(Frame, FrameSystem);
	private:
		float borderWidth;
		float cornerRadius;
		ColorRGBAf fillColor;
		ColorRGBAf borderColor; 

		friend class System;
	};
};