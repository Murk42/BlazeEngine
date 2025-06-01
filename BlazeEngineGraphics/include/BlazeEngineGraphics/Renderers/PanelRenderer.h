#pragma once
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineCore/Math/Vector.h"

namespace Blaze::Graphics
{
	struct PanelRenderData_OpenGL
	{
		Vec2f pos;
		Vec2f size;
		float rotation;
		float borderWidth;
		float cornerRadius;
		ColorRGBAf fillColor;
		ColorRGBAf borderColor;
	};
}