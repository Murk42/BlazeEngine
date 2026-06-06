#include "pch.h"
#include "BlazeEngine/UI/Graphics/Renderers/OpenGL/AntialiasedTextRenderer_OpenGL.h"
#include "BlazeEngine/UI/Graphics/Shaders/Shaders.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace Blaze::Graphics::OpenGL
{
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

	AntialiasedTextRenderer_OpenGL::AntialiasedTextRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(graphicsContext, bufferInstanceCount * sizeof(Instance)), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::antialiasedText_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::antialiasedText_frag);
			SetShaderProgram(ShaderProgram{ { vert, frag } });
		}

		va.SetVertexAttribute({ 0, &Instance::color, 1 });
		va.SetVertexAttribute({ 1, &Instance::p1, 1 });
		va.SetVertexAttribute({ 2, &Instance::p2, 1 });
		va.SetVertexAttribute({ 3, &Instance::p3, 1 });
		va.SetVertexAttribute({ 4, &Instance::uv1, 1 });
		va.SetVertexAttribute({ 5, &Instance::uv2, 1 });
		va.SetVertexAttribute({ 6, &Instance::blend, 1 });
		va.SetVertexAttribute({ 7, &Instance::alpha, 1 });
		va.SetVertexAttribute({ 8, &Instance::textureIndex, 1 });

		va.SetBindingBuffer(0, &GetBuffer(), sizeof(Instance), 0);
	}
	AntialiasedTextRenderer_OpenGL::~AntialiasedTextRenderer_OpenGL()
	{
		WaitFence();
	}
	void AntialiasedTextRenderer_OpenGL::StartRender(const RenderContext_OpenGL& renderContext)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);

		program.SetUniform(0, renderContext.OrthographicsProjectionMatrix(-1, 1));
	}
	void AntialiasedTextRenderer_OpenGL::EndRender()
	{
		Flush();
	}
	void AntialiasedTextRenderer_OpenGL::Render(const TextRectRenderData& data)
	{
		int textureIndex = 0;

		auto* texture = data.texture.GetValue<OpenGL::Texture2D>();

		if (texture == nullptr)
			textureIndex = -1;
		else
		{
			for (; textureIndex < _countof(textures) && textures[textureIndex] != texture; ++textureIndex)
				if (textures[textureIndex] == nullptr)
				{
					textures[textureIndex] = texture;
					break;
				}

			if (textureIndex == _countof(textures))
			{
				Flush();
				textureIndex = 0;
				textures[0] = texture;
			}
		}

		WaitFence();

		((Instance*)GetBufferMap())[instanceCount] = Instance{
			.color = data.color,
			.p1 = data.pos,
			.p2 = data.pos + data.right,
			.p3 = data.pos + data.up,
			.uv1 = Vec2f(data.uv1.x, data.uv2.y),
			.uv2 = Vec2f(data.uv2.x, data.uv1.y),
			.blend = data.blend,
			.alpha = data.alpha,
			.textureIndex = static_cast<float>(textureIndex),
		};

		instanceCount++;

		if ((instanceCount + 1) * sizeof(Instance) > GetBufferSize())
			Flush();
	}
	void AntialiasedTextRenderer_OpenGL::Flush()
	{
		if (instanceCount == 0)
			return;

		FlushBuffer(0, sizeof(Instance) * instanceCount);

		for (uint i = 0; i < _countof(textures) && textures[i] != nullptr; ++i)
		{
			program.SetUniform(1 + i, static_cast<int>(i));
			graphicsContext.SetActiveTextureSlot(i);
			graphicsContext.SelectTexture(textures[i]);
			textures[i] = nullptr;
		}

		graphicsContext.RenderInstancedPrimitiveArray(Blaze::Graphics::OpenGL::PrimitiveType::TriangleStrip, 0, 4, 0, instanceCount);
		fence.SetFence();

		instanceCount = 0;
	}
	void AntialiasedTextRenderer_OpenGL::WaitFence()
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