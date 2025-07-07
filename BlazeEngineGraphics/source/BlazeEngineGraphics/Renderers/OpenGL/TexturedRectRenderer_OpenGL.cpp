#include "pch.h"
#include "BlazeEngineCore/DataStructures/Map.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{	
	void TexturedRectRenderCache_OpenGL::CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData>& renderData)
	{						
		groups.Clear();

		struct TextureDivision
		{
			uintMem counter;
			Array<Instance> instances;
		};
		Map<const OpenGLWrapper::Texture2D*, TextureDivision> textures;

		for (auto& rd : renderData)
		{			
			TextureDivision td;
			td.counter = 0;

			auto it = textures.Insert(rd.texture.TryGet<OpenGLWrapper::Texture2D>(), std::move(td)).iterator;
			++it->value.counter;
		}

		for (auto& td : textures)
			td.value.instances.ReserveExactly(td.value.counter);

		for (auto& rd : renderData)
		{
			auto it = textures.Find(rd.texture.TryGet<OpenGLWrapper::Texture2D>());

			Instance instance{
				.color = rd.color,
				.p1 = rd.pos,
				.p2 = rd.pos + rd.right,
				.p3 = rd.pos + rd.up,
				.uv1 = rd.uv1,
				.uv2 = rd.uv2,
				.blend = rd.blend,
				.alpha = rd.alpha,
				.textureIndex = 0,
			};

			it->value.instances.AddBack(instance);
		}

		auto textureIt = textures.FirstIterator();
		uintMem textureInstancesProcessed = 0;

		List<Group>::Iterator lastGroupsIterator;
		auto AddGroup = [&]() -> Group& {
			if (groups.Empty())
				lastGroupsIterator = groups.AddFront();
			else
				lastGroupsIterator = groups.AddAfter(lastGroupsIterator);

			return *lastGroupsIterator;			
			};
		//At each iteration of this loop we fill a group
		while (true)
		{
			if (textureIt == textures.BehindIterator())
				break;					

			Group& group = AddGroup();
			memset(group.textures, 0, sizeof(group.textures));
									
			uintMem groupInstanceSpaceLeft = TexturedRectRenderer_OpenGL::InstanceBufferInstanceCount;

			//At each iteration of this loop we start to process one texture
			for (uintMem i = 0; i < TexturedRectRenderer_OpenGL::DrawCallTextureCount && textureIt != textures.BehindIterator(); ++i, ++textureIt)
			{
				group.textures[i] = textureIt->key;				
				auto& instances = textureIt->value.instances;				
				 
				uintMem copyCount = 0;

				if (groupInstanceSpaceLeft > instances.Count()) 
					//We can fit all the instances of the texture in the group
					copyCount = instances.Count() - textureInstancesProcessed;
				else
					//We cant fit all the instances of the texture in the group
					copyCount = groupInstanceSpaceLeft;
				
				for (uint j = 0; j < copyCount; ++j)
				{
					auto& dst = group.instances[j + TexturedRectRenderer_OpenGL::InstanceBufferInstanceCount - groupInstanceSpaceLeft];
					auto& src = instances[j + textureInstancesProcessed];					
										
					dst = src;					
				}				

				textureInstancesProcessed = copyCount;				
				groupInstanceSpaceLeft -= copyCount;

				if (groupInstanceSpaceLeft == 0)
					//The group is full so we exit early
					break;				
			}

			group.instanceCount = TexturedRectRenderer_OpenGL::InstanceBufferInstanceCount - groupInstanceSpaceLeft;
		}
	}	

	TexturedRectRenderer_OpenGL::TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, const TexturedRectRendererOptions_OpenGL& options)
		: graphicsContext(graphicsContext)
	{
		if (options.loadDefaultShaders)
		{
			auto vert = OpenGLWrapper::VertexShader(ShaderSources::texturedRect_vert);
			auto frag = OpenGLWrapper::FragmentShader(ShaderSources::texturedRect_frag);
			program.LinkShaders({ &vert, &frag });

			for (int i = 0; i < DrawCallTextureCount; ++i)
				program.SetUniform(1 + i, i);
		}

		instanceBuffer.Allocate(nullptr, InstanceBufferSize , OpenGLWrapper::GraphicsBufferMapAccessFlags::Write, Graphics::OpenGLWrapper::GraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 4, offsetof(Instance, color));
		va.SetVertexAttributeBuffer(0, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(0, 1);
		va.EnableVertexAttribute(1);
		va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, offsetof(Instance, p1));
		va.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);
		va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, offsetof(Instance, p2));
		va.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);
		va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, offsetof(Instance, p3));
		va.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);
		va.SetFloatVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, offsetof(Instance, uv1));
		va.SetVertexAttributeBuffer(4, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);
		va.EnableVertexAttribute(5);
		va.SetFloatVertexAttributeFormat(5, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, offsetof(Instance, uv2));
		va.SetVertexAttributeBuffer(5, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);
		va.EnableVertexAttribute(6);
		va.SetFloatVertexAttributeFormat(6, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, offsetof(Instance, blend));
		va.SetVertexAttributeBuffer(6, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
		va.EnableVertexAttribute(7);
		va.SetFloatVertexAttributeFormat(7, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, offsetof(Instance, alpha));
		va.SetVertexAttributeBuffer(7, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(7, 1);
		va.EnableVertexAttribute(8);
		va.SetFloatVertexAttributeFormat(8, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, offsetof(Instance, textureIndex));
		va.SetVertexAttributeBuffer(8, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(8, 1);
	}	
	void TexturedRectRenderer_OpenGL::Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{				
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		program.SetUniform(0, Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1));

		//Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		//fence.SetFence();

		for (auto& group : renderCache.groups)
		{
			//if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			//{
			//	Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
			//	return;
			//}

			void* instanceBufferMap = instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::ExplicitFlush);

			memcpy(instanceBufferMap, group.instances, sizeof(Instance) * group.instanceCount);
			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * group.instanceCount);
			instanceBuffer.UnmapBuffer();

			for (uint i = 0; i < DrawCallTextureCount && group.textures[i] != nullptr; ++i)
			{
				graphicsContext.SetActiveTextureSlot(i);
				graphicsContext.SelectTexture(group.textures[i]);
			}			

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, 0, group.instanceCount);

			//fence.SetFence();		
		}
	}
	void TexturedRectRenderer_OpenGL::Render(RenderStream& stream, Vec2u targetSize)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);		

		program.SetUniform(0, Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1));

		//Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		//fence.SetFence();
			
		stream.BeginStream();

		//When rd is nullptr it means next render data can be rendered, if not the one stored in rd should be
		TexturedRectRenderData* rd = nullptr;

		while (true)
		{
			//if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			//{
			//	Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
			//	return;
			//}

			const OpenGLWrapper::Texture2D* textures[DrawCallTextureCount]{ };

			Instance* instanceBufferMap = (Instance*)instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::ExplicitFlush);
			uint instanceCount = 0;

			while (true)
			{				
				rd = (TexturedRectRenderData*)stream.Advance();

				if (rd == nullptr)
					break;

				//
				//If textureIndex is set to -1 it means no texture will be used, if it is set to DrawCallTextureCount then there arent enough texture slots and the current batch will be rendered
				//
				int textureIndex = 0;

				auto* texture = rd->texture.TryGet<OpenGLWrapper::Texture2D>();

				if (texture == nullptr)
					textureIndex = -1;
				else
					for (; textureIndex < DrawCallTextureCount; ++textureIndex)
						if (textures[textureIndex] == texture)
							break;
						else if (textures[textureIndex] == nullptr)
						{
							textures[textureIndex] = texture;
							break;
						}

				if (textureIndex == DrawCallTextureCount)
					break;

				Instance instance{
				.color = rd->color,
				.p1 = rd->pos,
				.p2 = rd->pos + rd->right,
				.p3 = rd->pos + rd->up,
				.uv1 = Vec2f(rd->uv1.x, rd->uv2.y),
				.uv2 = Vec2f(rd->uv2.x, rd->uv1.y),
				.blend = rd->blend,
				.alpha = rd->alpha,
				.textureIndex = (float)textureIndex,
				};

				instanceBufferMap[instanceCount] = instance;				
				instanceCount++;

				if (instanceCount == InstanceBufferInstanceCount)
					break;
			}
			
			if (instanceCount == 0)
			{
				instanceBuffer.UnmapBuffer();
				break;
			}

			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * instanceCount);
			instanceBuffer.UnmapBuffer();

			for (uint i = 0; i < DrawCallTextureCount && textures[i] != nullptr; ++i)
			{
				graphicsContext.SetActiveTextureSlot(i);
				graphicsContext.SelectTexture(textures[i]);
			}

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);

			//fence.SetFence();

			if (rd == nullptr)
				break;
		}
	}
	void TexturedRectRenderer_OpenGL::SetShaderProgram(Blaze::Graphics::OpenGLWrapper::ShaderProgram&& program)
	{
		this->program = std::move(program);

		for (int i = 0; i < DrawCallTextureCount; ++i)
			this->program.SetUniform(1 + i, i);
	}

}