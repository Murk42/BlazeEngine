#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/Image.h"

namespace Blaze::UI::Nodes
{
	Image::Image() :
		renderUnit(this)
	{		
		dataMap.SetTypeName("Image");		
	}		
	Graphics::RenderUnit* Image::GetRenderUnit(uint index)
	{
		if (index == 0)
			return &renderUnit;

		return nullptr;
	}		
}