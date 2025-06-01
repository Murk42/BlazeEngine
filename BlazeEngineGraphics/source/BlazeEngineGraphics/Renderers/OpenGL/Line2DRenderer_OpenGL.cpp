#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLShader.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/Line2DRenderer_OpenGL.h"

#include "BlazeEngineGraphics/Shaders/Shaders.h"

namespace Blaze::Graphics::OpenGL
{
	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 32;

	Line2DRenderer_OpenGL::Line2DRenderer_OpenGL(GraphicsContext_OpenGL& graphicsContext)		
		: graphicsContext(graphicsContext)
	{
		auto vert = OpenGLWrapper::VertexShader(ShaderSources::line2d_vert);
		auto frag = OpenGLWrapper::FragmentShader(ShaderSources::line2d_frag);				
		program.LinkShaders({ &vert, &frag });		

		vb.Allocate(nullptr, VertexBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapAccessFlags::Write, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetFloatVertexAttributeFormat(0, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 0);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Line2DRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(1);
		va.SetFloatVertexAttributeFormat(1, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 8);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Line2DRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(2);
		va.SetFloatVertexAttributeFormat(2, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 2, 16);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Line2DRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(3);
		va.SetFloatVertexAttributeFormat(3, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 4, 24);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Line2DRenderCache_OpenGL::Vertex), 0);
		va.EnableVertexAttribute(4);
		va.SetFloatVertexAttributeFormat(4, Blaze::Graphics::OpenGLWrapper::FloatVertexAttributeType::Float, 1, 40);
		va.SetVertexAttributeBuffer(4, &vb, sizeof(Line2DRenderCache_OpenGL::Vertex), 0);
	}	
	void Line2DRenderer_OpenGL::CreateCache(const Array<Line2DRenderData_OpenGL>& renderData, Line2DRenderCache_OpenGL& cache)
	{
		cache.vertices.Resize(renderData.Count() * 6);

		uintMem i = 0;
		for (auto& rd : renderData)
		{		
			Vec2f pos1;
			Vec2f pos2;
			Vec2f pos3;
			Vec2f pos4;

			Vec2f dir = rd.p2 - rd.p1;
			if (dir.x == 0 && dir.y == 0)
			{
				pos1 = rd.p1 + Vec2f(rd.width, rd.width);
				pos2 = rd.p1 + Vec2f(rd.width, -rd.width);
				pos3 = rd.p1 + Vec2f(-rd.width, rd.width);
				pos4 = rd.p1 + Vec2f(-rd.width, -rd.width);
			}
			else
			{
				float len = dir.Lenght();
				dir = dir / len;
				Vec2f normal = Vec2f(dir.y, -dir.x);
				float half_width = rd.width;

				//+1 is to make the rectangle around the line wider so antialiased pixels will fit inside.
				pos1 = rd.p1 - dir * (half_width + 1) - normal * (half_width + 1);
				pos2 = rd.p2 + dir * (half_width + 1) - normal * (half_width + 1);
				pos3 = rd.p1 - dir * (half_width + 1) + normal * (half_width + 1);
				pos4 = rd.p2 + dir * (half_width + 1) + normal * (half_width + 1);
			}
			cache.vertices[i + 0] = { .pos = pos1, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			cache.vertices[i + 1] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			cache.vertices[i + 2] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			cache.vertices[i + 3] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			cache.vertices[i + 4] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			cache.vertices[i + 5] = { .pos = pos4, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };

			i += 6;
		}			
	}	
	void Line2DRenderer_OpenGL::Render(const Line2DRenderCache_OpenGL& renderCache, Vec2u targetSize)
	{		
		graphicsContext.SelectProgram(&program);
		graphicsContext.SelectVertexArray(&va);
		graphicsContext.SetActiveTextureSlot(0);

		Vec2u renderArea = Vec2u(targetSize);		
		Mat4f proj = Mat4f::OrthographicMatrix(0, (float)targetSize.x, 0, (float)targetSize.y, -1, 1);

		program.SetUniform(0, proj);

		Blaze::Graphics::OpenGLWrapper::Fence fence{ };
		fence.SetFence();
		uintMem offset = 0;
		while (offset != renderCache.vertices.Count())
		{
			uintMem count = renderCache.vertices.Count() % VectexBufferVertexCount;

			void* vbMap = vb.MapBufferRange(0, VertexBufferSize, Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::InvalidateBuffer | Blaze::Graphics::OpenGLWrapper::GraphicsBufferMapOptions::ExplicitFlush);
			
			memcpy(vbMap, renderCache.vertices.Ptr() + offset, sizeof(Line2DRenderCache_OpenGL::Vertex) * count);
			vb.FlushBufferRange(0, sizeof(Line2DRenderCache_OpenGL::Vertex) * count);
			vb.UnmapBuffer();			

			graphicsContext.RenderPrimitiveArray(Blaze::Graphics::OpenGLWrapper::PrimitiveType::Triangles, 0, count);

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