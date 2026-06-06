#include "pch.h"
#include "BlazeEngine/Graphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngine/Graphics/Shaders/Shaders.h"
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

	TexturedRectRenderer_OpenGL::TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext, uintMem bufferInstanceCount, bool loadDefaultShaders)
		: BufferedRendererBase_OpenGL(graphicsContext, bufferInstanceCount * sizeof(Instance)), instanceCount(0)
	{
		if (loadDefaultShaders)
		{
			auto vert = OpenGL::VertexShader(ShaderSources::texturedRect_vert);
			auto frag = OpenGL::FragmentShader(ShaderSources::texturedRect_frag);
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
		//va.EnableVertexAttribute(0);
		//va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 4, offsetof(Instance, color));
		//va.SetVertexAttributeDivisor(0, 1);
		//va.EnableVertexAttribute(1);
		//va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, offsetof(Instance, p1));
		//va.SetVertexAttributeDivisor(1, 1);
		//va.EnableVertexAttribute(2);
		//va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, offsetof(Instance, p2));
		//va.SetVertexAttributeDivisor(2, 1);
		//va.EnableVertexAttribute(3);
		//va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, offsetof(Instance, p3));
		//va.SetVertexAttributeDivisor(3, 1);
		//va.EnableVertexAttribute(4);
		//va.SetFloatVertexAttributeFormat(4, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, offsetof(Instance, uv1));
		//va.SetVertexAttributeDivisor(4, 1);
		//va.EnableVertexAttribute(5);
		//va.SetFloatVertexAttributeFormat(5, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 2, offsetof(Instance, uv2));
		//va.SetVertexAttributeDivisor(5, 1);
		//va.EnableVertexAttribute(6);
		//va.SetFloatVertexAttributeFormat(6, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, offsetof(Instance, blend));
		//va.SetVertexAttributeDivisor(6, 1);
		//va.EnableVertexAttribute(7);
		//va.SetFloatVertexAttributeFormat(7, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, offsetof(Instance, alpha));
		//va.SetVertexAttributeDivisor(7, 1);
		//va.EnableVertexAttribute(8);
		//va.SetFloatVertexAttributeFormat(8, Blaze::Graphics::OpenGL::FloatVertexAttributeType::Float, 1, offsetof(Instance, textureIndex));
		//va.SetVertexAttributeDivisor(8, 1);

		va.SetBindingBuffer(0, &GetBuffer(), sizeof(Instance), 0);
	}
	TexturedRectRenderer_OpenGL::~TexturedRectRenderer_OpenGL()
	{
		WaitFence();
	}
	void TexturedRectRenderer_OpenGL::StartRender(const RenderContext_OpenGL& renderContext)
	{
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		
		program.SetUniform(0, renderContext.OrthographicsProjectionMatrix(-1, 1));
	}
	void TexturedRectRenderer_OpenGL::EndRender()
	{
		Flush();
	}
	void TexturedRectRenderer_OpenGL::Render(const TexturedRectRenderData& data)
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
	void TexturedRectRenderer_OpenGL::Flush()
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
	void TexturedRectRenderer_OpenGL::WaitFence()
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