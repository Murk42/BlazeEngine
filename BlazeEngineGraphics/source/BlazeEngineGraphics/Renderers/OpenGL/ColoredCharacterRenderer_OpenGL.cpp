#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/ColoredCharacterRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Files/shaders.h"

namespace Blaze::Graphics::OpenGL
{
	ColoredCharacterRenderer_OpenGL::ColoredCharacterRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)
		: TexturedRectRenderer_OpenGL(graphicsContext, TexturedRectRendererOptions_OpenGL{ .loadDefaultShaders = false })
	{
		Blaze::Graphics::OpenGLWrapper::VertexShader vert;
		vert.ShaderSource(StringView((const char*)texturedRect_vert_file, texturedRect_vert_size));
		vert.CompileShader();
		 
		Blaze::Graphics::OpenGLWrapper::FragmentShader frag;
		frag.ShaderSource(StringView((const char*)text_frag_file, text_frag_size));
		frag.CompileShader();

		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		program.LinkShaders({ &vert, &frag });

		SetShaderProgram(std::move(program));
	}
}