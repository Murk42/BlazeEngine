#include "pch.h"
#include "TexturedRectRenderer_OpenGL.h"

namespace Blaze::Graphics::OpenGL
{
	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 40;

	void TexturedRectRenderCache_OpenGL::CreateNew(TexturedRectRenderer_OpenGL& renderer, const Array<TexturedRectRenderData_OpenGL>& renderData)
	{		
		groups.Clear();
		this->renderer = &renderer;

		for (auto& rd : renderData)
		{
			auto& vertices = groups.Insert(rd.texture).iterator->value;

			Vec2f p1 = rd.pos;
			Vec2f p2 = rd.pos + rd.right;
			Vec2f p3 = rd.pos + rd.up;
			Vec2f p4 = rd.pos + rd.right + rd.up;			

			Vertex v1 { .pos = { p1.x, p1.y }, .uv = { rd.uv1.x, rd.uv1.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v2 { .pos = { p2.x, p2.y }, .uv = { rd.uv2.x, rd.uv1.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v3 { .pos = { p3.x, p3.y }, .uv = { rd.uv1.x, rd.uv2.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v4 { .pos = { p4.x, p4.y }, .uv = { rd.uv2.x, rd.uv2.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };			

			vertices.ReserveAdditional(6);
			vertices.AddBack(v1);
			vertices.AddBack(v2);
			vertices.AddBack(v3);
			vertices.AddBack(v3);
			vertices.AddBack(v2);
			vertices.AddBack(v4);			
		}		
	}
	void TexturedRectRenderCache_OpenGL::Add(const ArrayView<TexturedRectRenderData_OpenGL>& renderData)
	{
		for (auto& rd : renderData)
		{
			auto& vertices = groups.Insert(rd.texture).iterator->value;

			Vec2f p1 = rd.pos;
			Vec2f p2 = rd.pos + rd.right;
			Vec2f p3 = rd.pos + rd.up;
			Vec2f p4 = rd.pos + rd.right + rd.up;

			Vertex v1{ .pos = { p1.x, p1.y }, .uv = { rd.uv1.x, rd.uv1.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v2{ .pos = { p2.x, p2.y }, .uv = { rd.uv2.x, rd.uv1.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v3{ .pos = { p3.x, p3.y }, .uv = { rd.uv1.x, rd.uv2.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };
			Vertex v4{ .pos = { p4.x, p4.y }, .uv = { rd.uv2.x, rd.uv2.y }, .color = rd.color, .blend = rd.blend, .alpha = rd.alpha };

			vertices.ReserveAdditional(6);
			vertices.AddBack(v1);
			vertices.AddBack(v2);
			vertices.AddBack(v3);
			vertices.AddBack(v3);
			vertices.AddBack(v2);
			vertices.AddBack(v4);
		}
	}


	TexturedRectRenderer_OpenGL::TexturedRectRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)		
	{
		Blaze::Graphics::OpenGLWrapper::VertexShader vert{ "assets/shaders/OpenGL/texturedRect.vert" };
		Blaze::Graphics::OpenGLWrapper::FragmentShader frag{ "assets/shaders/OpenGL/texturedRect.frag" };

		program.LinkShaders({ &vert, &frag });		

		vb.Allocate(nullptr, VertexBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(TexturedRectRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(1);
		va.SetVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 2, false, 8);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(TexturedRectRenderCache_OpenGL::Vertex), 0);				
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 4, false, 16);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(TexturedRectRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 32);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(TexturedRectRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::VertexAttributeType::Float, 1, false, 36);
		va.SetVertexAttributeBuffer(4, &vb, sizeof(TexturedRectRenderCache_OpenGL::Vertex), 0);
	}	
	void TexturedRectRenderer_OpenGL::Render(const TexturedRectRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{				
		Blaze::Graphics::OpenGLWrapper::SelectProgram(&program);
		Blaze::Graphics::OpenGLWrapper::SelectVertexArray(&va);
		Blaze::Graphics::OpenGLWrapper::SetActiveTextureSlot(0);		

		Vec2u renderArea = Vec2u(targetSize);		
		Mat4f proj = Mat4f::OrthographicMatrix(0, targetSize.x, 0, targetSize.y, -1, 1);

		Blaze::Graphics::OpenGLWrapper::SetActiveTextureSlot(0);

		program.SetUniform(0, proj);
		program.SetUniform(1, 0);

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();

		for (auto& group : renderCache.groups)
		{
			Blaze::Graphics::OpenGLWrapper::SelectTexture(group.key);

			uintMem offset = 0;
			while (offset != group.value.Count())
			{
				uintMem count = group.value.Count() % VectexBufferVertexCount;

				void* vbMap = vb.MapBufferRange(0, VertexBufferSize, Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);

				memcpy(vbMap, group.value.Ptr() + offset, sizeof(Vertex) * count);
				vb.FlushBufferRange(0, sizeof(Vertex) * count);
				vb.UnmapBuffer();

				Blaze::Graphics::OpenGLWrapper::RenderPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, count);

				fence.SetFence();

				if (fence.BlockClient(1.0) == Blaze::Graphics::OpenGLWrapper::FenceReturnState::TimeoutExpired)
				{
					Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
					return;
				}

				offset += count;
			}
		}
	}
}