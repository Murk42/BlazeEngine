#include "pch.h"
#include "Line2DRenderer_OpenGL.h"
#include "Line2DRenderTask_OpenGL.h"

namespace Graphics4::OpenGL
{
	static constexpr auto VectexBufferVertexCount = 256;
	static constexpr auto VertexBufferSize = VectexBufferVertexCount * 32;

	Line2DRenderer_OpenGL::Line2DRenderer_OpenGL(Graphics4::OpenGL::GraphicsContext_OpenGL& graphicsContext)
		: graphicsContext(graphicsContext)
	{
		OpenGLWrapper::VertexShader vert{ "assets/shaders/line2d.vert" };
		OpenGLWrapper::FragmentShader frag{ "assets/shaders/line2d.frag" };

		program.LinkShaders({ &vert, &frag });
		program.SetUniform(1, 0);

		vb.Allocate(nullptr, VertexBufferSize, OpenGLWrapper::ImmutableGraphicsBufferMapAccess::Write, OpenGLWrapper::ImmutableGraphicsBufferMapType::None);

		va.EnableVertexAttribute(0);
		va.SetVertexAttributeFormat(0, OpenGLWrapper::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
		va.EnableVertexAttribute(1);
		va.SetVertexAttributeFormat(1, OpenGLWrapper::VertexAttributeType::Float, 2, false, 8);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(2, OpenGLWrapper::VertexAttributeType::Float, 2, false, 16);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(3, OpenGLWrapper::VertexAttributeType::Float, 4, false, 24);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeFormat(4, OpenGLWrapper::VertexAttributeType::Float, 1, false, 40);
		va.SetVertexAttributeBuffer(4, &vb, sizeof(Vertex), 0);
	}	
	bool Line2DRenderer_OpenGL::CreateTask(const Array<Line2DRenderData>& renderData, Dynamic::Line2DRenderTaskBase& task)
	{
		if (auto taskPtr = task.TryCast<Line2DRenderTask_OpenGL>())
			return CreateTask(renderData, *taskPtr);

		if (!task.IsImplemented())
			return CreateTask(renderData, task.SetImplementation<Line2DRenderTask_OpenGL>());

		return false;
	}
	bool Line2DRenderer_OpenGL::CreateTask(const Array<Line2DRenderData>& renderData, Line2DRenderTask_OpenGL& task)
	{
		task.renderCache.vertices.Resize(renderData.Count() * 6);

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
			task.renderCache.vertices[i + 0] = { .pos = pos1, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			task.renderCache.vertices[i + 1] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			task.renderCache.vertices[i + 2] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			task.renderCache.vertices[i + 3] = { .pos = pos2, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			task.renderCache.vertices[i + 4] = { .pos = pos3, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };
			task.renderCache.vertices[i + 5] = { .pos = pos4, .pos1 = rd.p1, .pos2 = rd.p2, .color = rd.color, .width = rd.width };

			i += 6;
		}
		task.renderer = this;
		return false;
	}	
	void Line2DRenderer_OpenGL::Render(const RenderCache& renderCache, Graphics4::OpenGL::Framebuffer_OpenGL& renderTarget)
	{
		renderTarget.MakeActive();

		OpenGLWrapper::SelectProgram(&program);
		OpenGLWrapper::SelectVertexArray(&va);
		OpenGLWrapper::SetActiveTextureSlot(0);		

		Vec2u renderArea = Vec2u(renderTarget.GetSize());
		OpenGLWrapper::SetRenderArea(Vec2i(), (Vec2i)renderArea);
		Mat4f proj = Mat4f::OrthographicMatrix(0, renderArea.x, 0, renderArea.y, -1, 1);

		program.SetUniform(0, proj);

		OpenGLWrapper::Fence fence{ };
		fence.SetFence();
		uintMem offset = 0;
		while (offset != renderCache.vertices.Count())
		{
			uintMem count = renderCache.vertices.Count() % VectexBufferVertexCount;

			void* vbMap = vb.MapBufferRange(0, VertexBufferSize, OpenGLWrapper::ImmutableGraphicsBufferMapOptions::InvalidateBuffer | OpenGLWrapper::ImmutableGraphicsBufferMapOptions::ExplicitFlush);
			memcpy(vbMap, renderCache.vertices.Ptr() + offset, sizeof(Vertex) * count);
			vb.UnmapBuffer();			

			OpenGLWrapper::RenderPrimitiveArray(OpenGLWrapper::PrimitiveType::Triangles, 0, count);

			fence.SetFence();

			if (fence.BlockClient(1.0) == OpenGLWrapper::FenceReturnState::TimeoutExpired)
			{
				Debug::Logger::LogWarning("Blaze Graphics API", "Fence timeout");
				return;
			}

			offset += count;
		}
	}
}