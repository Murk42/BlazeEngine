#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/Renderer.h"

#include "source/BlazeEngine/Graphics/Shaders.h"
#include "BlazeEngine/Event/Events.h"
#include "BlazeEngine/Math/Math.h"

#include "BlazeEngine/Resources/Font/Font.h"

using namespace Blaze::Graphics::Core;
  
namespace Blaze
{
	
	constexpr size_t maxLine2DvertexCount = 128;
	constexpr size_t maxLine3DvertexCount = 128;
	constexpr size_t maxPoint2DvertexCount = 4096;

	struct Line2DVertex
	{
		Vec2f pos1;
		Vec2f pos2;
		Vec4f color;
		float width;
	};
	struct Line3DVertex
	{
		Vec3f pos1;
		Vec3f pos2;
		Vec4f color;
		float width;
	};
	struct Point2DVertex
	{
		Vec2f pos;
		Vec4f color;
		float radius;
	};
	struct GraphicsData
	{		
		bool userProj3D;

		Mat4f proj3D;
		Mat4f view3D;		
		Mat4f VP3D;
		Mat4f proj2D;
		
		Line2DVertex line2D_vertices[maxLine2DvertexCount];
		size_t line2D_vertexCount = 0;
		VertexArray line2D_va;
		GraphicsBuffer line2D_vb;
		Program line2D_program;
		int line2D_uniformMVP;
		bool line2D_renderable = true;

		Line3DVertex line3D_vertices[maxLine3DvertexCount];
		size_t line3D_vertexCount = 0;
		VertexArray line3D_va;
		GraphicsBuffer line3D_vb;
		Program line3D_program;
		int line3D_uniformVP3D;
		int line3D_uniformProj2D;
		int line3D_uniformViewportSize;
		bool line3D_renderable = true;

		Point2DVertex point2D_vertices[maxPoint2DvertexCount];
		size_t point2D_vertexCount = 0;
		VertexArray point2D_va;
		GraphicsBuffer point2D_vb;
		Program point2D_program;
		int point2D_uniformMVP;
		bool point2D_renderable = true;

		VertexArray text_va;
		GraphicsBuffer text_vb;
		Program text_program;
		Font text_font;
		bool text_renderable = true;
	};
	static GraphicsData* graphicsData;
	
	void Graphics_ViewportChanged(Event::ViewportChanged event)
	{
		graphicsData->proj2D = Math::OrthographicMatrix<float>(0, event.size.x, 0, event.size.y, -1, 1);
		if (!graphicsData->userProj3D)
			Graphics::Set3DProjectionMatrix(Math::PerspectiveMatrix<float>(90, Renderer::GetViewportRatio(), 0.1, 1000));				
	}
	
	void FlushDrawLine2D()
	{
		if (graphicsData->line2D_vertexCount == 0 || !graphicsData->line2D_renderable)
			return;

		graphicsData->line2D_vb.ChangeData({ graphicsData->line2D_vertices, graphicsData->line2D_vertexCount * sizeof(Line2DVertex) }, 0);
		Renderer::SelectVertexArray(&graphicsData->line2D_va);
		Renderer::SelectProgram(&graphicsData->line2D_program);
		graphicsData->line2D_program.SetUniform(graphicsData->line2D_uniformMVP, graphicsData->proj2D);

		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, graphicsData->line2D_vertexCount, 0);

		graphicsData->line2D_vertexCount = 0;
	}
	void FlushDrawLine3D()
	{
		if (graphicsData->line3D_vertexCount == 0 || !graphicsData->line3D_renderable)
			return;

		graphicsData->line3D_vb.ChangeData({ graphicsData->line3D_vertices, graphicsData->line3D_vertexCount * sizeof(Line3DVertex) }, 0);
		Renderer::SelectVertexArray(&graphicsData->line3D_va);
		Renderer::SelectProgram(&graphicsData->line3D_program);
		graphicsData->line3D_program.SetUniform(graphicsData->line3D_uniformVP3D, graphicsData->VP3D);
		graphicsData->line3D_program.SetUniform(graphicsData->line3D_uniformProj2D, graphicsData->proj2D);
		graphicsData->line3D_program.SetUniform(graphicsData->line3D_uniformViewportSize, Vec2f(Renderer::GetViewportSize()));

		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, graphicsData->line3D_vertexCount, 0);

		graphicsData->line3D_vertexCount = 0;
	}	
	void FlushDrawPoint2D()
	{
		if (graphicsData->point2D_vertexCount == 0 || !graphicsData->point2D_renderable)
			return;

		graphicsData->point2D_vb.ChangeData({ graphicsData->point2D_vertices, graphicsData->point2D_vertexCount * sizeof(Point2DVertex) }, 0);
		Renderer::SelectVertexArray(&graphicsData->point2D_va);
		Renderer::SelectProgram(&graphicsData->point2D_program);
		graphicsData->point2D_program.SetUniform(graphicsData->point2D_uniformMVP, graphicsData->proj2D);

		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Lines, graphicsData->point2D_vertexCount, 0);

		graphicsData->point2D_vertexCount = 0;
	}

	void InitializeLine2D()
	{
		VertexShader vert;
		GeometryShader geom;
		FragmentShader frag;
		if (vert.Load("assets/Blaze/Graphics/line2D.vert")) 
		{
			graphicsData->line2D_renderable = false; return;
		}
		if (geom.Load("assets/Blaze/Graphics/line2D.geom"))
		{
			graphicsData->line2D_renderable = false; return;
		}
		if (frag.Load("assets/Blaze/Graphics/line2D.frag"))
		{
			graphicsData->line2D_renderable = false; return;
		}

		graphicsData->line2D_program.LinkShaders({ &vert, &geom, &frag });
		graphicsData->line2D_uniformMVP = graphicsData->line2D_program.GetUniformLocation("u_MVP");

		graphicsData->line2D_va.EnableVertexAttribute(0);
		graphicsData->line2D_va.EnableVertexAttribute(1);
		graphicsData->line2D_va.EnableVertexAttribute(2);
		graphicsData->line2D_va.EnableVertexAttribute(3);
		graphicsData->line2D_va.SetVertexAttributeFormat(0, VertexAttributeType::Float, 2, false, 0);
		graphicsData->line2D_va.SetVertexAttributeFormat(1, VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		graphicsData->line2D_va.SetVertexAttributeFormat(2, VertexAttributeType::Float, 4, false, sizeof(float) * 4);
		graphicsData->line2D_va.SetVertexAttributeFormat(3, VertexAttributeType::Float, 1, false, sizeof(float) * 8);
		graphicsData->line2D_va.SetVertexAttributeBuffer(0, &graphicsData->line2D_vb, sizeof(Line2DVertex), 0);
		graphicsData->line2D_va.SetVertexAttributeBuffer(1, &graphicsData->line2D_vb, sizeof(Line2DVertex), 0);
		graphicsData->line2D_va.SetVertexAttributeBuffer(2, &graphicsData->line2D_vb, sizeof(Line2DVertex), 0);
		graphicsData->line2D_va.SetVertexAttributeBuffer(3, &graphicsData->line2D_vb, sizeof(Line2DVertex), 0);

		graphicsData->line2D_vb.AllocateStaticStorage({ nullptr, sizeof(Line2DVertex) * maxLine2DvertexCount }, GraphicsBufferStaticStorageHint::DynamicStorage);

		graphicsData->line2D_renderable = true;
	}
	void InitializeLine3D()
	{
		VertexShader vert;
		GeometryShader geom;
		FragmentShader frag;
		if (vert.Load("assets/Blaze/Graphics/line3D.vert"))
		{
			graphicsData->line3D_renderable = false; return;
		}
		if (geom.Load("assets/Blaze/Graphics/line3D.geom"))
		{
			graphicsData->line3D_renderable = false; return;
		}
		if (frag.Load("assets/Blaze/Graphics/line3D.frag"))
		{
			graphicsData->line3D_renderable = false; return;
		}
		graphicsData->line3D_program.LinkShaders({ &vert, &geom, &frag });
		graphicsData->line3D_uniformVP3D = graphicsData->line3D_program.GetUniformLocation("u_VP3D");
		graphicsData->line3D_uniformProj2D = graphicsData->line3D_program.GetUniformLocation("u_proj2D");
		graphicsData->line3D_uniformViewportSize = graphicsData->line3D_program.GetUniformLocation("u_viewportSize");

		graphicsData->line3D_va.EnableVertexAttribute(0);
		graphicsData->line3D_va.EnableVertexAttribute(1);
		graphicsData->line3D_va.EnableVertexAttribute(2);
		graphicsData->line3D_va.EnableVertexAttribute(3);
		graphicsData->line3D_va.SetVertexAttributeFormat(0, VertexAttributeType::Float, 3, false, 0);
		graphicsData->line3D_va.SetVertexAttributeFormat(1, VertexAttributeType::Float, 3, false, sizeof(float) * 3);
		graphicsData->line3D_va.SetVertexAttributeFormat(2, VertexAttributeType::Float, 4, false, sizeof(float) * 6);
		graphicsData->line3D_va.SetVertexAttributeFormat(3, VertexAttributeType::Float, 1, false, sizeof(float) * 10);
		graphicsData->line3D_va.SetVertexAttributeBuffer(0, &graphicsData->line3D_vb, sizeof(Line3DVertex), 0);
		graphicsData->line3D_va.SetVertexAttributeBuffer(1, &graphicsData->line3D_vb, sizeof(Line3DVertex), 0);
		graphicsData->line3D_va.SetVertexAttributeBuffer(2, &graphicsData->line3D_vb, sizeof(Line3DVertex), 0);
		graphicsData->line3D_va.SetVertexAttributeBuffer(3, &graphicsData->line3D_vb, sizeof(Line3DVertex), 0);

		graphicsData->line3D_vb.AllocateStaticStorage({ nullptr, sizeof(Line3DVertex) * maxLine3DvertexCount }, GraphicsBufferStaticStorageHint::DynamicStorage);
		graphicsData->line3D_renderable = true;
	}
	void InitializePoint2D()
	{
		VertexShader vert;
		GeometryShader geom;
		FragmentShader frag;
		if (vert.Load("assets/Blaze/Graphics/point2D.vert"))
		{
			graphicsData->point2D_renderable = false; return;
		}
		if (geom.Load("assets/Blaze/Graphics/point2D.geom"))
		{
			graphicsData->point2D_renderable = false; return;
		}
		if (frag.Load("assets/Blaze/Graphics/point2D.frag"))
		{
			graphicsData->point2D_renderable = false; return;
		}
		graphicsData->point2D_program.LinkShaders({ &vert, &geom, &frag });
		graphicsData->point2D_uniformMVP = graphicsData->point2D_program.GetUniformLocation("u_MVP");

		graphicsData->point2D_va.EnableVertexAttribute(0);
		graphicsData->point2D_va.EnableVertexAttribute(1);
		graphicsData->point2D_va.EnableVertexAttribute(2);
		graphicsData->point2D_va.SetVertexAttributeFormat(0, VertexAttributeType::Float, 2, false, 0);
		graphicsData->point2D_va.SetVertexAttributeFormat(1, VertexAttributeType::Float, 4, false, sizeof(float) * 2);
		graphicsData->point2D_va.SetVertexAttributeFormat(2, VertexAttributeType::Float, 1, false, sizeof(float) * 6);
		graphicsData->point2D_va.SetVertexAttributeBuffer(0, &graphicsData->point2D_vb, sizeof(Point2DVertex), 0);
		graphicsData->point2D_va.SetVertexAttributeBuffer(1, &graphicsData->point2D_vb, sizeof(Point2DVertex), 0);
		graphicsData->point2D_va.SetVertexAttributeBuffer(2, &graphicsData->point2D_vb, sizeof(Point2DVertex), 0);

		graphicsData->point2D_vb.AllocateStaticStorage({ nullptr, sizeof(Point2DVertex) * maxPoint2DvertexCount }, GraphicsBufferStaticStorageHint::DynamicStorage);

		graphicsData->point2D_renderable = true;
	}
	void InitializeText()
	{
		VertexShader vert;
		GeometryShader geom;
		FragmentShader frag;
		if (vert.Load("assets/Blaze/Graphics/text.vert"))
		{
			graphicsData->text_renderable = false; return;
		}
		if (geom.Load("assets/Blaze/Graphics/text.geom"))
		{
			graphicsData->text_renderable = false; return;
		}
		if (frag.Load("assets/Blaze/Graphics/text.frag"))
		{
			graphicsData->text_renderable = false; return;
		}

		graphicsData->text_program.LinkShaders({ &vert, &geom, &frag });

		graphicsData->text_va.EnableVertexAttribute(0);
		graphicsData->text_va.EnableVertexAttribute(1);
		graphicsData->text_va.EnableVertexAttribute(2);
		graphicsData->text_va.EnableVertexAttribute(3);
		graphicsData->text_va.SetVertexAttributeFormat(0, VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		graphicsData->text_va.SetVertexAttributeFormat(1, VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		graphicsData->text_va.SetVertexAttributeFormat(2, VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		graphicsData->text_va.SetVertexAttributeFormat(3, VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		graphicsData->text_va.SetVertexAttributeBuffer(0, &graphicsData->text_vb, sizeof(Font::CharacterVertex), 0);
		graphicsData->text_va.SetVertexAttributeBuffer(1, &graphicsData->text_vb, sizeof(Font::CharacterVertex), 0);
		graphicsData->text_va.SetVertexAttributeBuffer(2, &graphicsData->text_vb, sizeof(Font::CharacterVertex), 0);
		graphicsData->text_va.SetVertexAttributeBuffer(3, &graphicsData->text_vb, sizeof(Font::CharacterVertex), 0);

		graphicsData->text_renderable = true;

		graphicsData->text_font.Load("assets/Blaze/Consola.ttf", FontType::Antialiased, 32);
	}
	
	void InitializeGraphics()
	{
		graphicsData = new GraphicsData();		

		graphicsData->userProj3D = false;
		graphicsData->view3D = Mat4f::Identity();		

		InitializeLine2D();

		InitializeLine3D();

		InitializePoint2D();

		InitializeText();
	}
	void TerminateGraphics()
	{
		delete graphicsData;
	}

	namespace Graphics
	{
		void DrawLine2D(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width)
		{					
			if (!graphicsData->line2D_renderable)
				return;

			Line2DVertex vertex;
			vertex.pos1 = pos1;
			vertex.pos2 = pos2;
			vertex.color = ColorRGBAf(color).ToVector();
			vertex.width = width;

			graphicsData->line2D_vertices[graphicsData->line2D_vertexCount] = vertex;
			++graphicsData->line2D_vertexCount;

			if (graphicsData->line2D_vertexCount == maxLine2DvertexCount)
				FlushDrawLine2D();
		}
		void DrawBoxOutline2D(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width)
		{
			if (!graphicsData->line2D_renderable)
				return;

			Graphics::DrawLine2D(Vec2f(pos1.x, pos1.y), Vec2f(pos1.x, pos2.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos1.x, pos2.y), Vec2f(pos2.x, pos2.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos2.x, pos2.y), Vec2f(pos2.x, pos1.y), color, width);
			Graphics::DrawLine2D(Vec2f(pos2.x, pos1.y), Vec2f(pos1.x, pos1.y), color, width);
		}
		void DrawLine3D(Vec3f pos1, Vec3f pos2, ColorRGBA color, float width)
		{
			if (!graphicsData->line3D_renderable)
				return;

			Line3DVertex vertex;
			vertex.pos1 = pos1;
			vertex.pos2 = pos2;
			vertex.color = ColorRGBAf(color).ToVector();
			vertex.width = width;

			graphicsData->line3D_vertices[graphicsData->line3D_vertexCount] = vertex;
			++graphicsData->line3D_vertexCount;

			if (graphicsData->line3D_vertexCount == maxLine3DvertexCount)
				FlushDrawLine3D();
		}
		void DrawBoxOutline3D(Vec3f pos1, Vec3f pos2, ColorRGBA color, float width)
		{
			if (!graphicsData->line3D_renderable)
				return;

			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos2.y, pos1.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos2.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos2.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos1.y, pos1.z), Vec3f(pos1.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos1.y, pos1.z), Vec3f(pos2.x, pos1.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos1.x, pos2.y, pos1.z), Vec3f(pos1.x, pos2.y, pos2.z), color, width);
			Graphics::DrawLine3D(Vec3f(pos2.x, pos2.y, pos1.z), Vec3f(pos2.x, pos2.y, pos2.z), color, width);
		}
		void DrawPoint2D(Vec2f pos, ColorRGBA color, float radius)
		{
			if (!graphicsData->point2D_renderable)
				return;

			Point2DVertex vertex;
			vertex.pos = pos;			
			vertex.color = ColorRGBAf(color).ToVector();
			vertex.radius = radius;

			graphicsData->point2D_vertices[graphicsData->point2D_vertexCount] = vertex;
			++graphicsData->point2D_vertexCount;

			if (graphicsData->point2D_vertexCount == maxPoint2DvertexCount)
				FlushDrawPoint2D();
		}
		void Write(const StringViewUTF8& text, Vec2i pos, float height, ColorRGBA color)
		{ 
			uint vertexCount;
			Vec2f size;
			Font::CharacterVertex* vertices = graphicsData->text_font.GenerateVertices(text, vertexCount, size);
			graphicsData->text_vb.AllocateDynamicStorage(
				BufferView(vertices, vertexCount * sizeof(Font::CharacterVertex)), 
				Core::GraphicsBufferDynamicStorageHint::DynamicDraw
			);
			
			Renderer::SelectProgram(&graphicsData->text_program);
			Renderer::SelectVertexArray(&graphicsData->text_va);
			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(&graphicsData->text_font.GetTexture());
			graphicsData->text_program.SetUniform(0, 
				graphicsData->proj2D * 
				Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) *
				Math::ScalingMatrix<float>(Vec3f(size.x, size.y, 0) * height)
			);
			graphicsData->text_program.SetUniform(1, 0);
			graphicsData->text_program.SetUniform(2, ColorRGBAf(color).ToVector());
			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, vertexCount);

			return;
		}
		void Set3DViewMatrix(const Mat4f& mat)
		{
			graphicsData->view3D = mat;
			graphicsData->VP3D = graphicsData->proj3D * graphicsData->view3D;
		}		
		void Set3DProjectionMatrix(const Mat4f& mat)
		{
			graphicsData->userProj3D = true;
			graphicsData->proj3D = mat;
			graphicsData->VP3D = graphicsData->proj3D * graphicsData->view3D;
		}		
		Mat4f Get3DViewMatrix()
		{			
			return graphicsData->view3D;
		}
		Mat4f Get3DProjectionMatrix()
		{			
			return graphicsData->proj3D;
		}
		void Flush()
		{
			FlushDrawLine2D();
			FlushDrawLine3D();
			FlushDrawPoint2D();
		}
	}	
}