#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"

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
		: BufferedRendererBase_OpenGL(graphicsContext, bufferInstanceCount * sizeof(Instance)), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::panel_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::panel_frag);
			program = ShaderProgram({ vert, frag });
		}

		va.SetVertexAttribute({ 0, &Instance::fillColor, 1 });
		va.SetVertexAttribute({ 1, &Instance::borderColor, 1 });
		va.SetVertexAttribute({ 2, &Instance::p1, 1 });
		va.SetVertexAttribute({ 3, &Instance::p2, 1 });
		va.SetVertexAttribute({ 4, &Instance::p3, 1 });
		va.SetVertexAttribute({ 5, &Instance::borderWidth, 1 });
		va.SetVertexAttribute({ 6, &Instance::cornerRadius, 1 });

		va.SetBindingBuffer(0, &GetBuffer(), sizeof(Instance), 0);
	}
	PanelRenderer_OpenGL::~PanelRenderer_OpenGL()
	{
		WaitFence();
	}
	void PanelRenderer_OpenGL::StartRender(const RenderContext_OpenGL& renderContext)
	{
		instanceCount = 0;

		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);

		program.SetUniform(0, renderContext.OrthographicsProjectionMatrix(-1, 1));
	}
	void PanelRenderer_OpenGL::EndRender()
	{
		Flush();
	}
	void PanelRenderer_OpenGL::Render(const PanelRenderData& data)
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
			Flush();
	}
	void PanelRenderer_OpenGL::Flush()
	{
		if (instanceCount == 0)
			return;

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