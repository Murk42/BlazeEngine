#include "BlazeEngine/Graphics/Renderers/Line3DRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::Graphics
{
	static const char* vertSource =
		"#version 450									\n"
		"												\n"
		"layout(location = 0) in vec3 vert_pos1;		\n"
		"layout(location = 1) in vec3 vert_pos2;		\n"
		"layout(location = 2) in vec4 vert_color;		\n"
		"layout(location = 3) in float vert_width;		\n"
		"												\n"
		"out vec3 geom_pos1;							\n"
		"out vec3 geom_pos2;							\n"
		"out vec4 geom_color;							\n"
		"out float geom_width;							\n"
		"												\n"
		"void main()									\n"
		"{												\n"
		"	geom_pos1 = vert_pos1;						\n"
		"	geom_pos2 = vert_pos2;						\n"
		"	geom_color = vert_color;					\n"
		"	geom_width = vert_width;					\n"
		"}												\n";

	static const char* geomSource =
		"#version 450																	  \n"
		"																				  \n"
		"in vec3 geom_pos1[];															  \n"
		"in vec3 geom_pos2[];															  \n"
		"in vec4 geom_color[];															  \n"
		"in float geom_width[];															  \n"
		"																				  \n"
		"out vec4 frag_color;															  \n"
		"out vec2 frag_pos;																  \n"
		"out vec2 frag_pos1;															  \n"
		"out vec2 frag_pos2;															  \n"
		"out float frag_width;															  \n"
		"																				  \n"
		"layout(points) in;																  \n"
		"layout(line_strip, max_vertices = 2) out;										  \n"
		"																				  \n"
		"layout(location = 0) uniform mat4 u_VP3D;										  \n"
		"//layout(location = 1) uniform mat4 u_proj2D;									  \n"
		"//layout(location = 2) uniform vec2 u_viewportSize;							  \n"
		"																				  \n"
		"void main()																	  \n"
		"{																				  \n"
		"	//vec4 proj1 = u_VP3D * vec4(geom_pos1[0], 1);								  \n"
		"	//vec4 proj2 = u_VP3D * vec4(geom_pos2[0], 1);								  \n"
		"	//vec2 screenPos1 = (proj1.xy / proj1.w + 1.0f) * u_viewportSize / 2;		  \n"
		"	//vec2 screenPos2 = (proj2.xy / proj2.w + 1.0f) * u_viewportSize / 2;		  \n"
		"	//																			  \n"
		"	//vec2 dir = screenPos2.xy - screenPos1.xy;									  \n"
		"	//float len = length(dir);													  \n"
		"	//dir = dir / len;															  \n"
		"	//vec2 normal = vec2(dir.y, -dir.x);										  \n"
		"	//float half_width = geom_width[0] / 2;										  \n"
		"	//																			  \n"
		"	//vec2 pos;																	  \n"
		"	//pos = screenPos1.xy - dir * half_width - normal * half_width;				  \n"
		"	//gl_Position = proj1;														  \n"
		"	//frag_color = geom_color[0];												  \n"
		"	//frag_pos = pos;															  \n"
		"	//frag_pos1 = screenPos1.xy;												  \n"
		"	//frag_pos2 = screenPos2.xy;												  \n"
		"	//frag_width = half_width;													  \n"
		"	//EmitVertex();																  \n"
		"	//																			  \n"
		"	//pos = screenPos1.xy + dir * (half_width + len) - normal * half_width;		  \n"
		"	//gl_Position = proj2;														  \n"
		"	//frag_color = geom_color[0];												  \n"
		"	//frag_pos = pos;															  \n"
		"	//frag_pos1 = screenPos1.xy;												  \n"
		"	//frag_pos2 = screenPos2.xy;												  \n"
		"	//frag_width = half_width;													  \n"
		"	//EmitVertex();																  \n"
		"																				  \n"
		"	//pos = screenPos1.xy - dir * half_width + normal * half_width;				  \n"
		"	//gl_Position = proj1;														  \n"
		"	//frag_color = geom_color[0];												  \n"
		"	//frag_pos = pos;															  \n"
		"	//frag_pos1 = screenPos1.xy;												  \n"
		"	//frag_pos2 = screenPos2.xy;												  \n"
		"	//frag_width = half_width;													  \n"
		"	//EmitVertex();																  \n"
		"	//																			  \n"
		"	//pos = screenPos1.xy + dir * (half_width + len) + normal * half_width;		  \n"
		"	//gl_Position = proj2;														  \n"
		"	//frag_color = geom_color[0];												  \n"
		"	//frag_pos = pos;															  \n"
		"	//frag_pos1 = screenPos1.xy;												  \n"
		"	//frag_pos2 = screenPos2.xy;												  \n"
		"	//frag_width = half_width;													  \n"
		"	//EmitVertex();																  \n"
		"																				  \n"
		"	//EndPrimitive();															  \n"
		"																				  \n"
		"	gl_Position = u_VP3D * vec4(geom_pos1[0], 1);								  \n"
		"	frag_color = geom_color[0];													  \n"
		"	EmitVertex();																  \n"
		"	gl_Position = u_VP3D * vec4(geom_pos2[0], 1);								  \n"
		"	frag_color = geom_color[0];													  \n"
		"	EmitVertex();																  \n"
		"																				  \n"
		"	EndPrimitive();																  \n"
		"}																				  \n";

	static const char* fragSource =
		"#version 450															\n"
		"																		\n"
		"in vec4 frag_color;													\n"
		"in vec2 frag_pos;														\n"
		"in vec2 frag_pos1;														\n"
		"in vec2 frag_pos2;														\n"
		"in float frag_width;													\n"
		"																		\n"
		"float dist(vec2 p1, vec2 p2, vec2 p)									\n"
		"{																		\n"
		"	const vec2 dir = p2 - p1;											\n"
		"	const float l2 = dir.x * dir.x + dir.y * dir.y;						\n"
		"																		\n"
		"	if (l2 == 0.0) return distance(p, p1);								\n"
		"																		\n"
		"	const float t = max(0, min(1, dot(p - p1, dir) / l2));				\n"
		"	const vec2 projection = p1 + t * dir;								\n"
		"	return distance(p, projection);										\n"
		"}																		\n"
		"																		\n"
		"void main()															\n"
		"{																		\n"
		"	//float d = dist(frag_pos1, frag_pos2, frag_pos);					\n"
		"	//d = (d - frag_width + 2) / 2; 									\n"
		"	//d = clamp(d, 0, 1);												\n"
		"	//																	\n"
		"	//gl_FragColor = vec4(frag_color.rgb, mix(frag_color.a, 0, d));		\n"
		"	gl_FragColor = frag_color;											\n"
		"}																		\n";

	struct Vertex
	{
		Vec3f pos1;
		Vec3f pos2;
		Vec4f color;
		float width;
	};

	Line3DRenderer::Line3DRenderer()
		: cache(nullptr), batchSize(0)
	{
		Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
		Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
		Core::FragmentShader frag; frag.ShaderSource(fragSource); frag.CompileShader();

		program.LinkShaders({ &vert, &geom, &frag });

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 3, false, 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 3, false, sizeof(float) * 3);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 4, false, sizeof(float) * 6);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 1, false, sizeof(float) * 10);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
	}
	Line3DRenderer::~Line3DRenderer()
	{
		delete[] cache;
	}

	void Line3DRenderer::SetProjectionMatrix(Mat4f mat)
	{
		program.SetUniform(0, mat);
	}

	void Line3DRenderer::SetBatchMode(uint batchSize)
	{
		cache = new Vertex[batchSize];
		this->batchSize = batchSize;

		vb.AllocateDynamicStorage(BufferView(nullptr, batchSize * sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Line3DRenderer::SetImmediateMode()
	{
		delete[] cache;
		cache = nullptr;
		this->batchSize = 0;

		vb.AllocateDynamicStorage(BufferView(nullptr, sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Line3DRenderer::Draw(Vec3f pos1, Vec3f pos2, ColorRGBAf color, float width)
	{
		Vertex vertex;
		vertex.pos1 = pos1;
		vertex.pos2 = pos2;
		vertex.color = (Vec4f)color;
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
			vb.AllocateDynamicStorage(BufferView(&vertex, sizeof(Vertex)), Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
			Renderer::SelectVertexArray(&va);
			Renderer::SelectProgram(&program);
			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
		}
	}
	void Line3DRenderer::Flush()
	{
		vb.ChangeData(BufferView(cache, sizeof(Vertex) * batchSize), 0);
		Renderer::SelectVertexArray(&va);
		Renderer::SelectProgram(&program);
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, 1);
	}
}