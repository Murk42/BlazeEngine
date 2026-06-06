#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/Quad2DRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{
	struct Instance
	{
		Vec2f p1;
		Vec2f p2;
		Vec2f p3;
		Vec4f color;
	};

	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 32;


	Quad2DRenderer_OpenGL::Quad2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(graphicsContext, bufferInstanceCount * sizeof(Instance)), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			OpenGL::VertexShader vert{ ShaderSources::quad2d_vert };
			OpenGL::FragmentShader frag{ ShaderSources::quad2d_frag };
			SetShaderProgram(ShaderProgram{ { vert, frag } });
		}

		va.SetVertexAttribute({ 0, &Instance::p1, 1 });
		va.SetVertexAttribute({ 1, &Instance::p2, 1 });
		va.SetVertexAttribute({ 2, &Instance::p3, 1 });
		va.SetVertexAttribute({ 3, &Instance::color, 1});		
		va.SetBindingBuffer(0, &GetBuffer(), sizeof(Instance), 0);
	}
	Quad2DRenderer_OpenGL::~Quad2DRenderer_OpenGL()
	{
		WaitFence();
	}
	void Quad2DRenderer_OpenGL::StartRender(const RenderContext_OpenGL& renderContext)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);

		program.SetUniform(0, renderContext.OrthographicsProjectionMatrix(-1, 1));
	}
	void Quad2DRenderer_OpenGL::EndRender()
	{
		Flush();
	}
	void Quad2DRenderer_OpenGL::Render(const Quad2DRenderData& data)
	{
		WaitFence();

		((Instance*)GetBufferMap())[instanceCount] = Instance{
			.p1 = data.p1,
			.p2 = data.p2,
			.p3 = data.p3,
			.color = data.color
		};

		instanceCount++;

		if ((instanceCount + 1) * sizeof(Instance) > GetBufferSize())
			Flush();
	}
	void Quad2DRenderer_OpenGL::Flush()
	{
		if (instanceCount == 0)
			return;

		FlushBuffer(0, sizeof(Instance) * instanceCount);

		graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGL::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);
		fence.SetFence();

		instanceCount = 0;
	}
	void Quad2DRenderer_OpenGL::WaitFence()
	{
		switch (fence.BlockClient(2.0f))
		{
		case FenceReturnState::Error:
			BLAZE_LOG_ERROR("Error occured while waiting on a fence");
			break;
		default:
			BLAZE_LOG_ERROR("An invalid value was returned while waiting on a fence");
			break;
		case FenceReturnState::TimeoutExpired:
			BLAZE_LOG_ERROR("Timeout expired while waiting on a fence");
			break;
		case FenceReturnState::ConditionSatisfied:
		case FenceReturnState::AlreadySignaled:
		case FenceReturnState::FenceNotSet:
			break;
		}
	}
}