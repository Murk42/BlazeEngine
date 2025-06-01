#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/ColoredCharacterRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{
	ColoredCharacterRenderer_OpenGL::ColoredCharacterRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)
		: TexturedRectRenderer_OpenGL(graphicsContext, TexturedRectRendererOptions_OpenGL{ .loadDefaultShaders = false })
	{
		auto vert = OpenGLWrapper::VertexShader(ShaderSources::texturedRect_vert);
		auto frag = OpenGLWrapper::FragmentShader(ShaderSources::text_frag);				

		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		program.LinkShaders({ &vert, &frag });

		SetShaderProgram(std::move(program));
	}
}