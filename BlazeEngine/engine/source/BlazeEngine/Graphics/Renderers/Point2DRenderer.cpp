#include "BlazeEngine/Graphics/Renderers/Point2DRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::Graphics
{
	static const char* vertSource =
		"#version 450									  \n"
		"												  \n"
		"layout(location = 0) in vec2 vert_pos;			  \n"
		"layout(location = 1) in vec4 vert_color;		  \n"
		"layout(location = 2) in float vert_radius;		  \n"
		"												  \n"
		"out vec2 geom_pos;								  \n"
		"out vec4 geom_color;							  \n"
		"out float geom_radius;							  \n"
		"												  \n"
		"void main()									  \n"
		"{												  \n"
		"	geom_pos = vert_pos;						  \n"
		"	geom_color = vert_color;					  \n"
		"	geom_radius = vert_radius;					  \n"
		"}												  \n";
	static const char* geomSource =
		"#version 450																  \n"
		"																			  \n"
		"in vec2 geom_pos[];														  \n"
		"in vec4 geom_color[];														  \n"
		"in float geom_radius[];													  \n"
		"																			  \n"
		"out vec4 frag_color;														  \n"
		"out vec2 frag_pos;															  \n"
		"out vec2 frag_center;														  \n"
		"out float frag_radius;														  \n"
		"																			  \n"
		"layout(points) in;															  \n"
		"layout(triangle_strip, max_vertices = 4) out;								  \n"
		"																			  \n"
		"layout(location = 0) uniform mat4 u_MVP;									  \n"
		"																			  \n"
		"void main()																  \n"
		"{																			  \n"
		"	vec2 p1 = geom_pos[0] + vec2(-geom_radius[0] - 1, -geom_radius[0] - 1);	  \n"
		"	vec2 p2 = geom_pos[0] + vec2(-geom_radius[0] - 1, geom_radius[0] + 1);	  \n"
		"	vec2 p3 = geom_pos[0] + vec2(geom_radius[0] + 1, -geom_radius[0] - 1);	  \n"
		"	vec2 p4 = geom_pos[0] + vec2(geom_radius[0] + 1, geom_radius[0] + 1);	  \n"
		"																			  \n"
		"	gl_Position = u_MVP * vec4(p1, 0, 1);									  \n"
		"	frag_pos = p1;															  \n"
		"	frag_color = geom_color[0];												  \n"
		"	frag_center = geom_pos[0];												  \n"
		"	frag_radius = geom_radius[0];											  \n"
		"	EmitVertex();															  \n"
		"																			  \n"
		"	gl_Position = u_MVP * vec4(p2, 0, 1);									  \n"
		"	frag_pos = p2;															  \n"
		"	frag_color = geom_color[0];												  \n"
		"	frag_center = geom_pos[0];												  \n"
		"	frag_radius = geom_radius[0];											  \n"
		"	EmitVertex();															  \n"
		"																			  \n"
		"	gl_Position = u_MVP * vec4(p3, 0, 1);									  \n"
		"	frag_pos = p3;															  \n"
		"	frag_color = geom_color[0];												  \n"
		"	frag_center = geom_pos[0];												  \n"
		"	frag_radius = geom_radius[0];											  \n"
		"	EmitVertex();															  \n"
		"																			  \n"
		"	gl_Position = u_MVP * vec4(p4, 0, 1);									  \n"
		"	frag_pos = p4;															  \n"
		"	frag_color = geom_color[0];												  \n"
		"	frag_center = geom_pos[0];												  \n"
		"	frag_radius = geom_radius[0];											  \n"
		"	EmitVertex();															  \n"
		"																			  \n"
		"	EndPrimitive();															  \n"
		"}																			  \n";
	static const char* fragSource =
		"#version 450													  \n"
		"																  \n"
		"in vec4 frag_color;											  \n"
		"in vec2 frag_pos;												  \n"
		"in vec2 frag_center;											  \n"
		"in float frag_radius;											  \n"
		"																  \n"
		"void main()													  \n"
		"{																  \n"
		"	float d = distance(frag_pos, frag_center);					  \n"
		"	d = smoothstep(frag_radius - 1, frag_radius + 1, d);		  \n"
		"																  \n"
		"	gl_FragColor = vec4(frag_color.rgb, mix(frag_color.a, 0, d)); \n"
		"}																  \n";

	struct PointVertex
	{
		Vec2f pos;
		Vec4f color;
		float radius;
	};

	Point2DRenderer::Point2DRenderer()
		: cache(nullptr), batchSize(0)
	{
		Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
		Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
		Core::FragmentShader frag; frag.ShaderSource(fragSource); frag.CompileShader();

		program.LinkShaders({ &vert, &geom, &frag });

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 4, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 1, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(PointVertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(PointVertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(PointVertex), 0);
	}
	Point2DRenderer::~Point2DRenderer()
	{
		delete[] cache;
	}

	void Point2DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		program.SetUniform(0, mat);
	}

	void Point2DRenderer::SetBatchMode(uint batchSize)
	{
		cache = new PointVertex[batchSize];
		this->batchSize = batchSize;

		vb.AllocateDynamicStorage(BufferView(nullptr, batchSize * sizeof(PointVertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Point2DRenderer::SetImmediateMode()
	{
		delete[] cache;
		cache = nullptr;
		this->batchSize = 0;

		vb.AllocateDynamicStorage(BufferView(nullptr, sizeof(PointVertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Point2DRenderer::Draw(Vec2f pos, ColorRGBA color, float radius)
	{
		PointVertex vertex;
		vertex.pos = pos;
		vertex.color = color;
		vertex.radius = radius;

		if (cache != nullptr)
		{
			//((Vertex*)cache)[batchOffset] = vertex;
			//
			//batchOffset++;
			//
			//if (batchOffset == batchSize)
			//	Flush();
		}
		else
		{
			vb.ChangeData(BufferView(&vertex, sizeof(PointVertex)), 0);
			Renderer::SelectVertexArray(&va);
			Renderer::SelectProgram(&program);
			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
		}
	}
	void Point2DRenderer::Flush()
	{
		vb.ChangeData(BufferView(cache, sizeof(PointVertex) * batchSize), 0);
		Renderer::SelectVertexArray(&va);
		Renderer::SelectProgram(&program);
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
	}
}