#pragma once
#include "BlazeEngineGraphics/Core/StreamRenderer.h"

namespace Blaze::Graphics::OpenGL
{	
	struct TexturedRectRenderData_OpenGL
	{
		OpenGLWrapper::Texture2D* texture;
		Vec2f uv1, uv2;
		Vec2f pos, right, up;		
		ColorRGBAf color;
		float blend ;
		float alpha ;
	};

	class TexturedRectRenderCache_OpenGL;	

	class TexturedRectRenderer_OpenGL : public StreamRenderer
	{
	public:
		TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext);
		
		void Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize);
		void Render(RenderStream& renderStream, Vec2u targetSize);		
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
			float blend;
			float alpha;
			float textureIndex;
		};

		constexpr static uintMem DrawCallTextureCount = 4;
		static constexpr auto InstanceBufferInstanceCount = 128;
		static constexpr auto InstanceBufferSize = InstanceBufferInstanceCount * sizeof(Instance);

		Blaze::Graphics::OpenGLWrapper::ShaderProgram program; 
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;
		Blaze::Graphics::OpenGLWrapper::ImmutableStaticGraphicsBuffer vertexBuffer;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class TexturedRectRenderCache_OpenGL;
	};

	class TexturedRectRenderCache_OpenGL
	{	
	public:	
		void CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData_OpenGL>& renderData);		
	private:		
		using Instance = TexturedRectRenderer_OpenGL::Instance;
		using Vertex = TexturedRectRenderer_OpenGL::Vertex;

		struct Group
		{
			OpenGLWrapper::Texture2D* textures[TexturedRectRenderer_OpenGL::DrawCallTextureCount];
			Instance instances[TexturedRectRenderer_OpenGL::InstanceBufferInstanceCount];
			uint instanceCount;
		};

		List<Group> groups;		

		friend class TexturedRectRenderer_OpenGL;
	public:		
	};		
}