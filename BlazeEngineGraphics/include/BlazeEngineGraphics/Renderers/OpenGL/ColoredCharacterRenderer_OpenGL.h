#pragma once
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	class BLAZE_GRAPHICS_API ColoredCharacterRenderer_OpenGL : public TexturedRectRenderer_OpenGL
	{
	public:
		ColoredCharacterRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);		

		StringView GetRendererName() const override { return "ColoredCharacterRenderer_OpenGL"; };	
	};
}