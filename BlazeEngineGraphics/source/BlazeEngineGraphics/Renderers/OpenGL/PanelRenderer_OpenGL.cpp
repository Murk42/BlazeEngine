#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Files/shaders.h"

namespace Blaze::Graphics::OpenGL
{
	static constexpr auto InstanceBufferInstanceCount = 256;
	static constexpr auto InstanceBufferSize = InstanceBufferInstanceCount * 64; //64 is sizeof(PanelRenderCache_OpenGL::Instance)

	void PanelRenderCache_OpenGL::CreateNew(PanelRenderer_OpenGL& renderer, const Array<PanelRenderData_OpenGL>& renderData)
	{		
		instances.Clear();

		for (auto& rd : renderData)
		{			
			float s = Math::Sin(rd.rotation);
			float c = Math::Cos(rd.rotation);

			Vec2f right = Vec2f(c, s);
			Vec2f up = Vec2f(-s, c);

			Vec2f p1 = rd.pos;
			Vec2f p2 = p1 + right * rd.size.x;
			Vec2f p3 = p1 + up * rd.size.y;

			Instance instance{ 
				.fillColor = rd.fillColor, .borderColor = rd.borderColor,
				.p1 = p1, .p2 = p2, .p3 = p3,
				.borderWidth = rd.borderWidth, .cornerRadius = rd.cornerRadius
			};						
			instances.AddBack(instance);
		}
	}
	PanelRenderer_OpenGL::PanelRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
	{
		Blaze::Graphics::OpenGLWrapper::VertexShader vert;
		vert.ShaderSource(StringView((const char*)panel_vert_file, panel_vert_size));
		vert.CompileShader();

		Blaze::Graphics::OpenGLWrapper::FragmentShader frag;
		frag.ShaderSource(StringView((const char*)panel_frag_file, panel_frag_size));
		frag.CompileShader();

		program.LinkShaders({ &vert, &frag });

		uint8 vertices[6]{
			0, 1, 2, 2, 1, 3
		};

		vertexBuffer.Allocate(vertices, sizeof(vertices));
		instanceBuffer.Allocate(nullptr, InstanceBufferSize, Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Uint8, 1, false, 0);
		va.SetVertexAttributeBuffer(0, &vertexBuffer, sizeof(PanelRenderCache_OpenGL::Vertex), 0);

		va.EnableVertexAttribute(1);		
		va.SetVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 4, false, 0);
		va.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);		
		va.SetVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 4, false, 16);
		va.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);		
		va.SetVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 32);
		va.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);		
		va.SetVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 40);
		va.SetVertexAttributeBuffer(4, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);
		va.EnableVertexAttribute(5);		
		va.SetVertexAttributeFormat(5, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 48);
		va.SetVertexAttributeBuffer(5, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);		
		va.EnableVertexAttribute(6);		
		va.SetVertexAttributeFormat(6, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 56);
		va.SetVertexAttributeBuffer(6, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
		va.EnableVertexAttribute(7);
		va.SetVertexAttributeFormat(7, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 60);
		va.SetVertexAttributeBuffer(7, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(7, 1);
	}
	void PanelRenderer_OpenGL::Render(const PanelRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{
		using Instance = PanelRenderCache_OpenGL::Instance;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		Vec2u renderArea = Vec2u(targetSize);
		Mat4f proj = Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1);

		graphicsContext.SetActiveTextureSlot(0);

		program.SetUniform(0, proj);		

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();

		uintMem offset = 0;
		while (offset != renderCache.instances.Count())
		{
			uintMem count = renderCache.instances.Count() % InstanceBufferInstanceCount;

			void* instanceBufferMap = instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);

			memcpy(instanceBufferMap, renderCache.instances.Ptr() + offset, sizeof(Instance) * count);
			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * count);
			instanceBuffer.UnmapBuffer();

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, 0, renderCache.instances.Count());

			fence.SetFence();

			if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			offset += count;
		}
	}
	void PanelRenderer_OpenGL::Render(RenderStream& renderStream, Vec2u targetSize)
	{ 
		using Instance = PanelRenderCache_OpenGL::Instance;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		Vec2u renderArea = Vec2u(targetSize);
		Mat4f proj = Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1);		

		program.SetUniform(0, proj);

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();

		renderStream.BeginStream();

		PanelRenderData_OpenGL* rd = nullptr;
		
		while (true)
		{		
			if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			Instance* instanceBufferMap = (Instance*)instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);			
			uintMem instanceCount = 0;

			while (true)
				//!renderStream.IsEmpty() && instanceCount < InstanceBufferInstanceCount)
			{
				rd = (PanelRenderData_OpenGL*)renderStream.Advance();

				if (rd == nullptr)
					break;

				float cos = Math::Cos(rd->rotation);
				float sin = Math::Sin(rd->rotation);
				Vec2f right = Vec2f(cos, sin) * rd->size.x;
				Vec2f up = Vec2f(-sin, cos) * rd->size.y;

				Instance instance = {
					.fillColor = rd->fillColor,
					.borderColor = rd->borderColor,
					.p1 = rd->pos,
					.p2 = rd->pos + right,
					.p3 = rd->pos + up,
					.borderWidth = rd->borderWidth,
					.cornerRadius = rd->cornerRadius,
				};

				instanceBufferMap[instanceCount] = instance;
				++instanceCount;


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

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, 6, 0, instanceCount);

			fence.SetFence();			

			if (rd == nullptr)
				break;
		}
	}
}