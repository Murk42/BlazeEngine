#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineCore/BlazeEngineCore.h"

namespace Blaze::UI
{
	struct BLAZE_GRAPHICS_API NodeFinalTransform
	{
		Vec2f position;
		Vec2f size;
		float scale = 1.0f;
		float rotation = 0.0f;

		Vec2f TransformFromFinalToLocalTransformSpace(Vec2f finalTransformPosition);
	};
}