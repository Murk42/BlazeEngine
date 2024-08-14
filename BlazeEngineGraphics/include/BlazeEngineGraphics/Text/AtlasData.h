#pragma once
//TODO remove dependance from OpenGL
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace Blaze::Graphics
{
	struct AtlasData
	{
		struct UVRect
		{
			Vec2f uv1;
			Vec2f uv2;
		};

		OpenGLWrapper::Texture2D atlas;
		Map<UnicodeChar, UVRect> uvs;
		uint fontHeight;
	};
}