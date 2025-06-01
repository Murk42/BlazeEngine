#pragma once
#include "BlazeEngineCore/Math/Vector.h"
#include "BlazeEngineCore/DataStructures/Color.h"
//TODO remove OpenGL texture dependance

namespace Blaze::Graphics
{
	namespace OpenGLWrapper
	{
		class Texture2D;
	}

	//The RGB channels of the texture and the color are mixed using the blend paramater.
	//The A channel of the texture and the color are multiplied together with the alpha parameter.
	struct TexturedRectRenderData
	{
		const OpenGLWrapper::Texture2D* texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;
		ColorRGBAf color;
		float blend;
		float alpha;
	};
}