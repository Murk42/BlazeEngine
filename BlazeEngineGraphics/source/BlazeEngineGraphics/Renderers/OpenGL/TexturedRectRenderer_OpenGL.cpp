#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{	
	void TexturedRectRenderCache_OpenGL::CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData_OpenGL>& renderData)
	{						
		groups.Clear();

		struct TextureDivision
		{
			uintMem counter;
			Array<Instance> instances;
		};
		Map<OpenGLWrapper::Texture2D*, TextureDivision> textures;

		for (auto& rd : renderData)
		{			
			TextureDivision td;
			td.counter = 0;

			auto it = textures.Insert(rd.texture, std::move(td)).iterator;
			++it->value.counter;
		}

		for (auto& td : textures)
			td.value.instances.ReserveExactly(td.value.counter);

		for (auto& rd : renderData)
		{
			auto it = textures.Find(rd.texture);

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

		//At each iteration of this loop we fill a group
		while (true)
		{
			if (textureIt == textures.BehindIterator())
				break;					

			Group& group = *groups.AddBack();
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

					dst.textureIndex = i;
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

	TexturedRectRenderer_OpenGL::TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)		
	{
		Blaze::Graphics::OpenGLWrapper::VertexShader vert{ "assets/shaders/OpenGL/texturedRect.vert" };
		Blaze::Graphics::OpenGLWrapper::FragmentShader frag{ "assets/shaders/OpenGL/texturedRect.frag" };

		program.LinkShaders({ &vert, &frag });		

		uint8 vertices[6]{
			0, 1, 2, 2, 1, 3
		};

		vertexBuffer.Allocate(vertices, sizeof(vertices));
		instanceBuffer.Allocate(nullptr, InstanceBufferSize, OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint8, 1, false, 0);
		va.SetVertexAttributeBuffer(0, &vertexBuffer, sizeof(Vertex), 0);

		va.EnableVertexAttribute(1);
		va.SetVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 4, false, 0);
		va.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 16);
		va.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 24);
		va.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 32);
		va.SetVertexAttributeBuffer(4, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);
		va.EnableVertexAttribute(5);
		va.SetVertexAttributeFormat(5, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 40);
		va.SetVertexAttributeBuffer(5, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);
		va.EnableVertexAttribute(6);
		va.SetVertexAttributeFormat(6, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 48);
		va.SetVertexAttributeBuffer(6, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
		va.EnableVertexAttribute(7);
		va.SetVertexAttributeFormat(7, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 56);
		va.SetVertexAttributeBuffer(7, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(7, 1);
		va.EnableVertexAttribute(8);
		va.SetVertexAttributeFormat(8, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 60);
		va.SetVertexAttributeBuffer(8, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(8, 1);
		va.EnableVertexAttribute(9);
		va.SetVertexAttributeFormat(9, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 64);
		va.SetVertexAttributeBuffer(9, &instanceBuffer, sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(9, 1);
	}	
	void TexturedRectRenderer_OpenGL::Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{				
		Blaze::Graphics::OpenGLWrapper::SelectProgram(&program);
		Blaze::Graphics::OpenGLWrapper::SelectVertexArray(&va);
		Blaze::Graphics::OpenGLWrapper::SetActiveTextureSlot(0);		

		Vec2u renderArea = Vec2u(targetSize);		
		Mat4f proj = Mat4f::OrthographicMatrix(0, targetSize.x, 0, targetSize.y, -1, 1);

		program.SetUniform(0, proj);

		for (uint i = 0; i < DrawCallTextureCount; ++i)
			program.SetUniform(1 + i, i);	

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();

		for (auto& group : renderCache.groups)
		{
			if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			void* instanceBufferMap = instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);

			memcpy(instanceBufferMap, group.instances, sizeof(Instance) * group.instanceCount);
			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * group.instanceCount);
			instanceBuffer.UnmapBuffer();

			for (uint i = 0; i < DrawCallTextureCount && group.textures[i] != nullptr; ++i)
			{
				OpenGLWrapper::SetActiveTextureSlot(i);
				OpenGLWrapper::SelectTexture(group.textures[i]);
			}			

			Blaze::Graphics::OpenGLWrapper::RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, group.instanceCount);

			fence.SetFence();		
		}
	}
	void TexturedRectRenderer_OpenGL::Render(RenderStream& stream, Vec2u targetSize)
	{
		Blaze::Graphics::OpenGLWrapper::SelectProgram(&program);
		Blaze::Graphics::OpenGLWrapper::SelectVertexArray(&va);
		Blaze::Graphics::OpenGLWrapper::SetActiveTextureSlot(0);

		Vec2u renderArea = Vec2u(targetSize);
		Mat4f proj = Mat4f::OrthographicMatrix(0, targetSize.x, 0, targetSize.y, -1, 1);

		program.SetUniform(0, proj);
		program.SetUniform(1, 0);
		program.SetUniform(2, 1);
		program.SetUniform(3, 2);
		program.SetUniform(4, 3);

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();
	
		stream.BeginStream();

		while (!stream.IsEmpty())
		{
			if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			OpenGLWrapper::Texture2D* textures[DrawCallTextureCount]{ };

			Instance* instanceBufferMap = (Instance*)instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);
			uint instanceCount = 0;

			while (true)
			{
				TexturedRectRenderData_OpenGL& rd = *(TexturedRectRenderData_OpenGL*)stream.GetCurrent();

				int textureIndex = -1;
				while (true)
				{
					if (rd.texture == nullptr || textureIndex == DrawCallTextureCount || textures[textureIndex] == rd.texture)
						break;
					else if (textures[textureIndex] == nullptr)
					{
						textures[textureIndex] = rd.texture;						
						break;
					}					

					++textureIndex;
				}				

				if (textureIndex == DrawCallTextureCount)
					break;

				Instance instance{
				.color = rd.color,
				.p1 = rd.pos,
				.p2 = rd.pos + rd.right,
				.p3 = rd.pos + rd.up,
				.uv1 = rd.uv1,
				.uv2 = rd.uv2,
				.blend = rd.blend,
				.alpha = rd.alpha,
				.textureIndex = (float)textureIndex,
				};

				instanceBufferMap[instanceCount++] = instance;

				stream.Advance();

				if (instanceCount == InstanceBufferInstanceCount || stream.IsEmpty())
					break;
			}
			
			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * instanceCount);
			instanceBuffer.UnmapBuffer();

			for (uint i = 0; i < DrawCallTextureCount && textures[i] != nullptr; ++i)
			{
				OpenGLWrapper::SetActiveTextureSlot(i);
				OpenGLWrapper::SelectTexture(textures[i]);
			}

			Blaze::Graphics::OpenGLWrapper::RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, instanceCount);

			fence.SetFence();
		}
	}	
}