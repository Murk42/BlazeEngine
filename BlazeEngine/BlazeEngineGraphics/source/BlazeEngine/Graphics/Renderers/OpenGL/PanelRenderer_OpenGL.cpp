#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{
	struct Instance
	{
		Vec4f fillColor;
		Vec4f borderColor;
		Vec2f p1;
		Vec2f p2;
		Vec2f p3;
		float borderWidth;
		float cornerRadius;
	};

	PanelRenderer_OpenGL::PanelRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(bufferInstanceCount * sizeof(Instance), graphicsContext), graphicsContext(graphicsContext), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::panel_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::panel_frag);
			program = ShaderProgram({ vert, frag });
		}

		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 4, 0);
		va.SetVertexAttributeBuffer(0, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(0, 1);
		va.EnableVertexAttribute(1);
		va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 4, 16);
		va.SetVertexAttributeBuffer(1, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(1, 1);
		va.EnableVertexAttribute(2);
		va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, 32);
		va.SetVertexAttributeBuffer(2, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(2, 1);
		va.EnableVertexAttribute(3);
		va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, 40);
		va.SetVertexAttributeBuffer(3, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(3, 1);
		va.EnableVertexAttribute(4);
		va.SetFloatVertexAttributeFormat(4, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, 48);
		va.SetVertexAttributeBuffer(4, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(4, 1);
		va.EnableVertexAttribute(5);
		va.SetFloatVertexAttributeFormat(5, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, 56);
		va.SetVertexAttributeBuffer(5, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(5, 1);
		va.EnableVertexAttribute(6);
		va.SetFloatVertexAttributeFormat(6, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, 60);
		va.SetVertexAttributeBuffer(6, &GetBuffer(), sizeof(Instance), 0);
		va.SetVertexAttributeDivisor(6, 1);
	}
	PanelRenderer_OpenGL::~PanelRenderer_OpenGL()
	{
		WaitFence();
	}
	void PanelRenderer_OpenGL::StartRender(const RenderContext& context)
	{
		instanceCount = 0;
	}
	void PanelRenderer_OpenGL::EndRender(const RenderContext& context)
	{
		Flush(context);
	}
	void PanelRenderer_OpenGL::Render(const PanelRenderData& data, const RenderContext& context)
	{
		WaitFence();
		((Instance*)GetBufferMap())[instanceCount] = {
			.fillColor = data.fillColor,
			.borderColor = data.borderColor,
			.p1 = data.pos,
			.p2 = data.pos + data.right,
			.p3 = data.pos + data.up,
			.borderWidth = data.borderWidth,
			.cornerRadius = data.cornerRadius,
		};

		instanceCount++;

		if ((instanceCount + 1) * sizeof(Instance) > GetBufferSize())
			Flush(context);
	}
	void PanelRenderer_OpenGL::Flush(const RenderContext& context)
	{
		if (instanceCount == 0)
			return;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);
		program.SetUniform(0, Mat4f::OrthographicMatrix(0, static_cast<float>(context.GetTargetSize().x), 0, static_cast<float>(context.GetTargetSize().y), -1, 1));

		FlushBuffer(0, sizeof(Instance) * instanceCount);
		graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGL::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);

		fence.SetFence();
	}
	void PanelRenderer_OpenGL::WaitFence()
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