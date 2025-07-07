#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/DataStructures/Color.h"
#include "BlazeEngineCore/Resource/ResourceRef.h"

namespace Blaze::Graphics
{	
	//The RGB channels of the texture and the color are mixed using the blend paramater.
	//The A channel of the texture and the color are multiplied together with the alpha parameter.
	struct TexturedRectRenderData
	{
		ResourceBaseRef texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;
		ColorRGBAf color;
		float blend;
		float alpha;
	};
}