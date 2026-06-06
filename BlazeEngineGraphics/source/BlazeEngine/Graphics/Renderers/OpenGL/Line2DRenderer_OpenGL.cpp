#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{
	struct Instance
	{
		Vec2f p1;
		Vec2f p2;
		Vec4f color;
		float width;
	};

	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 32;


	Line2DRenderer_OpenGL::Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(graphicsContext, bufferInstanceCount * sizeof(Instance)), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			OpenGL::VertexShader vert{ ShaderSources::line2d_vert };
			OpenGL::FragmentShader frag{ ShaderSources::line2d_frag };
			SetShaderProgram(ShaderProgram{ { vert, frag } });
		}

		va.SetVertexAttribute({ 0, &Instance::p1, 1 });
		va.SetVertexAttribute({ 1, &Instance::p2, 1 });
		va.SetVertexAttribute({ 2, &Instance::color, 1 });
		va.SetVertexAttribute({ 3, &Instance::width, 1 });		

		va.SetBindingBuffer(0, &GetBuffer(), sizeof(Instance), 0);
	}
	Line2DRenderer_OpenGL::~Line2DRenderer_OpenGL()
	{
		WaitFence();
	}
	void Line2DRenderer_OpenGL::StartRender(const RenderContext_OpenGL& renderContext)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);		

		program.SetUniform(0, renderContext.OrthographicsProjectionMatrix(-1, 1));
	}
	void Line2DRenderer_OpenGL::EndRender()
	{
		Flush();
	}
	void Line2DRenderer_OpenGL::Render(const Line2DRenderData& data)
	{
		WaitFence();

		((Instance*)GetBufferMap())[instanceCount] = Instance{
			.p1 = data.p1,
			.p2 = data.p2,
			.color = data.color,
			.width = data.width
		};

		instanceCount++;

		if ((instanceCount + 1) * sizeof(Instance) > GetBufferSize())
			Flush();
	}
	void Line2DRenderer_OpenGL::Flush()
	{
		if (instanceCount == 0)
			return;

		FlushBuffer(0, sizeof(Instance) * instanceCount);

		graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGL::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);
		fence.SetFence();

		instanceCount = 0;
	}
	void Line2DRenderer_OpenGL::WaitFence()
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