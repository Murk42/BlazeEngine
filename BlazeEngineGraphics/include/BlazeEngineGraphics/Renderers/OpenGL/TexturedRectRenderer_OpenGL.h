#pragma once
#include "BlazeEngineCore/DataStructures/List.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/RenderStructure/StreamRenderer.h"
#include "BlazeEngineGraphics/Core/OpenGL/GraphicsContext_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLProgram.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLVertexArray.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLGraphicsBuffer.h"

namespace Blaze::Graphics::OpenGL
{		
	class TexturedRectRenderCache_OpenGL;

	struct TexturedRectRendererOptions_OpenGL
	{
		bool loadDefaultShaders : 1 = true;
	};

	class BLAZE_GRAPHICS_API TexturedRectRenderer_OpenGL : public StreamRenderer
	{
	public:		
		TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, const TexturedRectRendererOptions_OpenGL& options = TexturedRectRendererOptions_OpenGL());
		
		void Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize);
		void Render(RenderStream& renderStream, Vec2u targetSize);		

		void SetShaderProgram(Blaze::Graphics::OpenGLWrapper::ShaderProgram&& program);
		
		StringView GetRendererName() const override { return "TexturedRectRenderer_OpenGL"; }
	private:				
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

		static constexpr uintMem DrawCallTextureCount = 4;
		static constexpr uintMem InstanceBufferInstanceCount = 128;
		static constexpr uintMem InstanceBufferSize = InstanceBufferInstanceCount * sizeof(Instance);

		GraphicsContext_OpenGL& graphicsContext;
		Blaze::Graphics::OpenGLWrapper::ShaderProgram program; 
		Blaze::Graphics::OpenGLWrapper::ImmutableMappedGraphicsBuffer instanceBuffer;
		Blaze::Graphics::OpenGLWrapper::VertexArray va;

		friend class TexturedRectRenderCache_OpenGL;
	};

	class BLAZE_GRAPHICS_API TexturedRectRenderCache_OpenGL
	{	
	public:	
		void CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData>& renderData);		
	private:		
		using Instance = TexturedRectRenderer_OpenGL::Instance;

		struct Group
		{
			const OpenGLWrapper::Texture2D* textures[TexturedRectRenderer_OpenGL::DrawCallTextureCount];
			Instance instances[TexturedRectRenderer_OpenGL::InstanceBufferInstanceCount];
			uint instanceCount;
		};

		List<Group> groups;		

		friend class TexturedRectRenderer_OpenGL;
	public:		
	};		
}