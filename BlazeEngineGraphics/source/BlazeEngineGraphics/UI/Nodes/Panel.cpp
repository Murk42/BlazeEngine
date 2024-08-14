#include "pch.h"
#include "BlazeEngineGraphics/UI/Nodes/Panel.h"

namespace Blaze::UI
{
	Panel::Panel() :
		renderUnit(this)
	{
		dataMap.SetTypeName("Panel");				
	}	
	Graphics::RenderUnit* Panel::GetRenderUnit(uint index)
	{
		if (index == 0)
			return &renderUnit;
		return nullptr;
	}	
}