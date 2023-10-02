#include "BlazeEngine/Application/UI/Components/FrameComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"

namespace Blaze::UI::Components
{
	void FrameSystem::Render(UIScene* scene, UIComponent* component)
	{
		Frame& frame = *(Frame*)component;		

		//auto& frameRenderer = Graphics::Core::GetDefaultFrameRenderer();
		//
		//Graphics::FrameGraphicsData data {.size = frame.transform->GetFinalSize(), .fillColor = frame.fillColor, .borderColor = frame.borderColor, .borderWidth = frame.borderWidth, .cornerRadius = frame.cornerRadius};
		//
		//frameRenderer.Render(frame.transform->GetMatrix(), data);
	}

	Frame::Frame(const Frame& other)
		: borderWidth(other.borderWidth), cornerRadius(other.cornerRadius), fillColor(other.fillColor), borderColor(other.borderColor)
	{
	}
	Frame::Frame(ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius)
		: borderWidth(borderWidth), cornerRadius(cornerRadius), fillColor(fillColor), borderColor(borderColor)
	{ 
	} 
	void Frame::Setup(ColorRGBAf fillColor, ColorRGBAf borderColor, float borderWidth, float cornerRadius)
	{
		this->fillColor = fillColor;
		this->borderColor = borderColor;
		this->borderWidth = borderWidth;
		this->cornerRadius = cornerRadius;
	}
	void Frame::SetBorderWidth(float borderWidth)
	{
		this->borderWidth = borderWidth;
	}
	void Frame::SetCornerRadius(float cornerRadius)
	{
		this->cornerRadius = cornerRadius;
	}
	void Frame::SetFillColor(ColorRGBAf fillColor)
	{
		this->fillColor = fillColor;
	}
	void Frame::SetBorderColor(ColorRGBAf borderColor)
	{
		this->borderColor = borderColor;
	}	
}
