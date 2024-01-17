#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphics.h"

namespace Blaze::Graphics::OpenGL
{
	struct TextRenderData_OpenGL
	{
		struct CharacterData
		{
			UnicodeChar character;
			ColorRGBAf color;
			Vec2f pos;
			Vec2f size;
		};

		Vec2f pos;
		float rotation;
		
		Array<CharacterData> characterData;
		float atlasFontHeight;
		Font* font;		
	};

	class TextRenderer_OpenGL;

	class TextRenderCache_OpenGL
	{
	public:
		void CreateNew(TextRenderer_OpenGL& renderer, const Array<TextRenderData_OpenGL>& renderData);

	private:
		struct Vertex
		{
			uint8 sign;
		};
		struct Instance
		{
			Vec4f color;			
			Vec2f p1;
			Vec2f p2;
			Vec2f p3;
			Vec2f uv1;
			Vec2f uv2;
		};

		Map<OpenGLWrapper::Texture2D*, Array<Instance>> groups;		

		friend class TextRenderer_OpenGL;
	};

	class TextRenderer_OpenGL
	{
	public:
		TextRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);

		void Render(const TextRenderCache_OpenGL& renderCache, Vec2u targetSize);
	private:
		using Vertex = TextRenderCache_OpenGL::Vertex;
		using Instance = TextRenderCache_OpenGL::Instance;

		Blaze::Graphics::OpenGLWrapper::ShaderProgram program;
		Blaze::Graphics::OpenGLWrapper::ImmutableStaticGraphicsBuffer vertexBuffer;
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class TextRenderCache_OpenGL;
	};
}