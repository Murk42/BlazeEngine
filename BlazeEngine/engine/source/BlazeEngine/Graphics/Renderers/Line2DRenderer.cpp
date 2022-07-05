#include "BlazeEngine/Graphics/Renderers/Line2DRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Console/Console.h"

namespace Blaze::Graphics
{
	static const char* vertSource =
		"#version 450								 \n"
		"											 \n"
		"layout(location = 0) in vec2 vert_pos1;	 \n"
		"layout(location = 1) in vec2 vert_pos2;	 \n"
		"layout(location = 2) in vec4 vert_color;	 \n"
		"layout(location = 3) in float vert_width;	 \n"
		"											 \n"
		"out vec2 geom_pos1;						 \n"
		"out vec2 geom_pos2;						 \n"
		"out vec4 geom_color;						 \n"
		"out float geom_width;						 \n"
		"											 \n"
		"void main()								 \n"
		"{											 \n"
		"	geom_pos1 = vert_pos1;					 \n"
		"	geom_pos2 = vert_pos2;					 \n"
		"	geom_color = vert_color;				 \n"
		"	geom_width = vert_width;				 \n"
		"}											 \n";
	static const char* geomSource =
		"#version 450																			 \n"
		"																						 \n"
		"in vec2 geom_pos1[];																	 \n"
		"in vec2 geom_pos2[];																	 \n"
		"in vec4 geom_color[];																	 \n"
		"in float geom_width[];																	 \n"
		"																						 \n"
		"out vec4 frag_color;																	 \n"
		"out vec2 frag_pos;																		 \n"
		"out vec2 frag_pos1;																	 \n"
		"out vec2 frag_pos2;																	 \n"
		"out float frag_width;																	 \n"
		"																						 \n"
		"layout(points) in;																		 \n"
		"layout(triangle_strip, max_vertices = 4) out;											 \n"
		"																						 \n"
		"layout(location = 0) uniform mat4 u_MVP;												 \n"
		"																						 \n"
		"void main()																			 \n"
		"{																						 \n"
		"	vec2 dir = geom_pos2[0] - geom_pos1[0];												 \n"
		"	float len = length(geom_pos2[0] - geom_pos1[0]);									 \n"
		"	dir = dir / len;																	 \n"
		"	vec2 normal = vec2(dir.y, -dir.x);													 \n"
		"	float half_width = geom_width[0];													 \n"
		"																						 \n"
		"	vec2 pos1 = geom_pos1[0] - dir * (half_width + 1) - normal * (half_width + 1);		 \n"
		"	vec2 pos2 = geom_pos1[0] + dir * (half_width + len + 1) - normal * (half_width + 1); \n"
		"	vec2 pos3 = geom_pos1[0] - dir * (half_width + 1) + normal * (half_width + 1);		 \n"
		"	vec2 pos4 = geom_pos1[0] + dir * (half_width + len + 1) + normal * (half_width + 1); \n"
		"																						 \n"
		"	gl_Position = u_MVP * vec4(pos1, 0, 1);												 \n"
		"	frag_color = geom_color[0];															 \n"
		"	frag_pos = pos1;																	 \n"
		"	frag_pos1 = geom_pos1[0];															 \n"
		"	frag_pos2 = geom_pos2[0];															 \n"
		"	frag_width = geom_width[0];															 \n"
		"	EmitVertex();																		 \n"
		"																						 \n"
		"	gl_Position = u_MVP * vec4(pos2, 0, 1);												 \n"
		"	frag_color = geom_color[0];															 \n"
		"	frag_pos = pos2;																	 \n"
		"	frag_pos1 = geom_pos1[0];															 \n"
		"	frag_pos2 = geom_pos2[0];															 \n"
		"	frag_width = geom_width[0];															 \n"
		"	EmitVertex();																		 \n"
		"																						 \n"
		"	gl_Position = u_MVP * vec4(pos3, 0, 1);												 \n"
		"	frag_color = geom_color[0];															 \n"
		"	frag_pos = pos3;																	 \n"
		"	frag_pos1 = geom_pos1[0];															 \n"
		"	frag_pos2 = geom_pos2[0];															 \n"
		"	frag_width = geom_width[0];															 \n"
		"	EmitVertex();																		 \n"
		"																						 \n"
		"	gl_Position = u_MVP * vec4(pos4, 0, 1);												 \n"
		"	frag_color = geom_color[0];															 \n"
		"	frag_pos = pos4;																	 \n"
		"	frag_pos1 = geom_pos1[0];															 \n"
		"	frag_pos2 = geom_pos2[0];															 \n"
		"	frag_width = geom_width[0];															 \n"
		"	EmitVertex();																		 \n"
		"																						 \n"
		"	EndPrimitive();																		 \n"
		"}																						 \n";
	static const char* fragSource =
		"#version 450															   \n"
		"																		   \n"
		"in vec4 frag_color;													   \n"
		"in vec2 frag_pos;														   \n"
		"in vec2 frag_pos1;														   \n"
		"in vec2 frag_pos2;														   \n"
		"in float frag_width;													   \n"
		"																		   \n"
		"float dist(vec2 p1, vec2 p2, vec2 p)									   \n"
		"{																		   \n"
		"	const vec2 dir = p2 - p1;											   \n"
		"	const float l2 = dir.x * dir.x + dir.y * dir.y;						   \n"
		"																		   \n"
		"	if (l2 == 0.0) return distance(p, p1);								   \n"
		"																		   \n"
		"	const float t = max(0, min(1, dot(p - p1, dir) / l2));				   \n"
		"	const vec2 projection = p1 + t * dir;								   \n"
		"	return distance(p, projection);										   \n"
		"}																		   \n"
		"																		   \n"
		"void main()															   \n"
		"{																		   \n"
		"	float d = dist(frag_pos1, frag_pos2, frag_pos);						   \n"
		"	d = smoothstep(frag_width - 1, frag_width + 1, d);					   \n"
		"																		   \n"
		"	gl_FragColor = vec4(vec3(frag_color.rgb), mix(frag_color.a, 0, d));     \n"
		"}																		   \n";

	struct Vertex
	{
		Vec2f pos1;
		Vec2f pos2;
		Vec4f color;
		float width = 0.0f;
	};

	Line2DRenderer::Line2DRenderer()
		: cache(nullptr), batchSize(0), batchOffset(0)
	{		
		Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
		Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
		Core::FragmentShader frag; frag.ShaderSource(fragSource); frag.CompileShader();

		program.LinkShaders({ &vert, &geom, &frag });

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 4, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 1, false, sizeof(float) * 8);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
	}
	Line2DRenderer::~Line2DRenderer()
	{
		delete[] cache;
	}

	void Line2DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		program.SetUniform(0, mat);
	}

	void Line2DRenderer::SetBatchMode(uint batchSize)
	{
		cache = new Vertex[batchSize];
		this->batchSize = batchSize;

		vb.AllocateDynamicStorage(BufferView(nullptr, batchSize * sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Line2DRenderer::SetImmediateMode()
	{
		delete[] cache;
		cache = nullptr;
		this->batchSize = 0;

		vb.AllocateDynamicStorage(BufferView(nullptr, sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Line2DRenderer::Draw(Vec2f pos1, Vec2f pos2, ColorRGBA color, float width)
	{
		Vertex vertex;
		vertex.pos1 = pos1;
		vertex.pos2 = pos2;
		vertex.color = color;
		vertex.width = width;

		if (cache != nullptr)
		{
			((Vertex*)cache)[batchOffset] = vertex;

			batchOffset++;

			if (batchOffset == batchSize)
				Flush();
		}
		else
		{
			vb.ChangeData(BufferView(&vertex, sizeof(Vertex)), 0);
			Renderer::SelectVertexArray(&va);
			Renderer::SelectProgram(&program);
			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
		}
	}
	void Line2DRenderer::Flush()
	{
		vb.ChangeData(BufferView(cache, sizeof(Vertex) * batchSize), 0);
		Renderer::SelectVertexArray(&va);
		Renderer::SelectProgram(&program);
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
	}
}