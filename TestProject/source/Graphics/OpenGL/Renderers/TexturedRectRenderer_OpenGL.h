#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"
#include "Resources/ResourceManager.h"

namespace Blaze::Graphics::OpenGL
{	
	struct TexturedRectRenderData_OpenGL
	{
		Graphics::OpenGLWrapper::Texture2D* texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;		
		ColorRGBAf color;
		float blend ;
		float alpha ;
	};

	class TexturedRectRenderer_OpenGL;
	
	class TexturedRectRenderCache_OpenGL
	{	
	public:		
		void CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData_OpenGL>& renderData);

		void Add(const ArrayView<TexturedRectRenderData_OpenGL>& renderData);
	private:
		struct Vertex
		{
			Vec2f pos;			
			Vec2f uv;			
			Vec4f color;
			float blend;
			float alpha;
		};

		TexturedRectRenderer_OpenGL* renderer;
		Map<Graphics::OpenGLWrapper::Texture2D*, Array<Vertex>> groups;		

		friend class TexturedRectRenderer_OpenGL;
	};	

	class TexturedRectRenderer_OpenGL
	{
	public:
		TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		
		void Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize);
	private:				
		using Vertex = TexturedRectRenderCache_OpenGL::Vertex;

		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer vb;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class TexturedRectRenderCache_OpenGL;
	};
}