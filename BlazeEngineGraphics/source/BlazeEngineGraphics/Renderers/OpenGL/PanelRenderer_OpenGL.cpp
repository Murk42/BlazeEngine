#include "pch.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Shaders/Shaders.h"

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
		auto vert = OpenGLWrapper::VertexShader(ShaderSources::panel_vert);
		auto frag = OpenGLWrapper::FragmentShader(ShaderSources::panel_frag);		
		program.LinkShaders({ &vert, &frag });

		instanceBuffer.Allocate(nullptr, InstanceBufferSize, Graphics::OpenGLWrapper::GraphicsBufferMapAccessFlags::Write, Graphics::OpenGLWrapper::GraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);		
		va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 4, 0);
		va.SetVertexAttributeBuffer(0, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(0, 1);
		va.EnableVertexAttribute(1);		
		va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 4, 16);
		va.SetVertexAttributeBuffer(1, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);		
		va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 32);
		va.SetVertexAttributeBuffer(2, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);		
		va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 40);
		va.SetVertexAttributeBuffer(3, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);		
		va.SetFloatVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 48);
		va.SetVertexAttributeBuffer(4, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);		
		va.EnableVertexAttribute(5);		
		va.SetFloatVertexAttributeFormat(5, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, 56);
		va.SetVertexAttributeBuffer(5, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);
		va.EnableVertexAttribute(6);
		va.SetFloatVertexAttributeFormat(6, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, 60);
		va.SetVertexAttributeBuffer(6, &instanceBuffer, sizeof(PanelRenderCache_OpenGL::Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
	}
	void PanelRenderer_OpenGL::Render(const PanelRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{
		using Instance = PanelRenderCache_OpenGL::Instance;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);		

		graphicsContext.SetActiveTextureSlot(0);

		program.SetUniform(0, Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1));

		//Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		//fence.SetFence();

		uintMem offset = 0;
		while (offset != renderCache.instances.Count())
		{
			uintMem count = renderCache.instances.Count() % InstanceBufferInstanceCount;

			void* instanceBufferMap = instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::ExplicitFlush);

			memcpy(instanceBufferMap, renderCache.instances.Ptr() + offset, sizeof(Instance) * count);
			instanceBuffer.FlushBufferRange(0, sizeof(Instance) * count);
			instanceBuffer.UnmapBuffer();

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::TriangleStrip, 0, 4, 0, renderCache.instances.Count());

			//fence.SetFence();
			//
			//if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			//{
			//	Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
			//	return;
			//}

			offset += count;
		}
	}
	void PanelRenderer_OpenGL::Render(RenderStream& renderStream, Vec2u targetSize)
	{ 
		using Instance = PanelRenderCache_OpenGL::Instance;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		program.SetUniform(0, Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1));

		//Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		//fence.SetFence();

		renderStream.BeginStream();

		PanelRenderData_OpenGL* rd = nullptr;
		
		while (true)
		{		
			//if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
			//{
			//	Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
			//	return;
			//}

			Instance* instanceBufferMap = (Instance*)instanceBuffer.MapBufferRange(0, InstanceBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::ExplicitFlush);			
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

			graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);

			//fence.SetFence();			

			if (rd == nullptr)
				break;
		}
	}
}