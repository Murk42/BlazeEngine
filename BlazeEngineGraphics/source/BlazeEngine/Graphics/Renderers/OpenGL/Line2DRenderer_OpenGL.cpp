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
		: BufferedRendererBase_OpenGL(bufferInstanceCount * sizeof(Instance), graphicsContext), graphicsContext(graphicsContext), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			OpenGL::VertexShader vert{ ShaderSources::line2d_vert };
			OpenGL::FragmentShader frag{ ShaderSources::line2d_frag };
			SetShaderProgram(ShaderProgram{ { vert, frag } });
		}

		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, 0);
		va.SetVertexAttributeBuffer(0, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(0, 1);
		va.EnableVertexAttribute(1);
		va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, 8);
		va.SetVertexAttributeBuffer(1, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);
		va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 4, 16);
		va.SetVertexAttributeBuffer(2, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);
		va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, 32);
		va.SetVertexAttributeBuffer(3, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
	}
	Line2DRenderer_OpenGL::~Line2DRenderer_OpenGL()
	{
		WaitFence();
	}
	void Line2DRenderer_OpenGL::StartRender(const RenderContext& context)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);

		Mat4f proj = Mat4f::OrthographicMatrix(0, (float)context.GetTargetSize().x, 0, (float)context.GetTargetSize().y, -1, 1);

		program.SetUniform(0, proj);
	}
	void Line2DRenderer_OpenGL::EndRender(const RenderContext& context)
	{
		Flush(context);
	}
	void Line2DRenderer_OpenGL::Render(const Line2DRenderData& data, const RenderContext& context)
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
			Flush(context);
	}
	void Line2DRenderer_OpenGL::Flush(const RenderContext& context)
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