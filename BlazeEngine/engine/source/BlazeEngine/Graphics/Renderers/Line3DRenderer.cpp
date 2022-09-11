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
		"	geom_width = max(vert_width, 1.5f);			\n"
		"}												\n";

	static const char* geomSource =
		"#version 460														 \n"
		"																	 \n"
		"layout(points) in;													 \n"
		"layout(triangle_strip, max_vertices = 4) out;						 \n"
		"																	 \n"
		"in vec3 geom_pos1[];												 \n"
		"in vec3 geom_pos2[];												 \n"
		"in vec4 geom_color[];												 \n"
		"in float geom_width[];												 \n"
		"																	 \n"
		"out vec4 frag_color;												 \n"
		"out vec2 frag_pos;													 \n"
		"out vec2 frag_pos1;												 \n"
		"out vec2 frag_pos2;												 \n"
		"out float frag_width;												 \n"
		"																	 \n"
		"layout(location = 0) uniform mat4 u_VP3D;							 \n"
		"layout(location = 1) uniform vec2 u_viewportSize;					 \n"
		"																	 \n"
		"bool isInCube(vec3 pos)											 \n"
		"{																	 \n"
		"	return															 \n"
		"		pos.x >= -1 && pos.x <= 1 &&								 \n"
		"		pos.y >= -1 && pos.y <= 1 &&								 \n"
		"		pos.z >= -1 && pos.z <= 1;									 \n"
		"}																	 \n"
		"																	 \n"
		"void swap(inout vec3 a, inout vec3 b)								 \n"
		"{																	 \n"
		"	vec3 t = a;														 \n"
		"	a = b;															 \n"
		"	b = t;															 \n"
		"}																	 \n"
		"																	 \n"
		"void main()														 \n"
		"{																	 \n"
		"	vec4 p1 = u_VP3D * vec4(geom_pos1[0], 1);						 \n"
		"	vec4 p2 = u_VP3D * vec4(geom_pos2[0], 1);						 \n"
		"																	 \n"
		"	vec3 ndc1 = p1.xyz / p1.w;										 \n"
		"	vec3 ndc2 = p2.xyz / p2.w;										 \n"
		"																	 \n"
		"	bool inside1 = isInCube(ndc1);									 \n"
		"	bool inside2 = isInCube(ndc2);									 \n"
		"																	 \n"
		"	if (!inside1 || !inside2)										 \n"
		"	{																 \n"
		"		if (ndc2.x < ndc1.x)										 \n"
		"			swap(ndc1, ndc2);										 \n"
		"																	 \n"
		"		vec3 vec = normalize(ndc2 - ndc1);							 \n"
		"																	 \n"
		"		if (ndc2.x < -1 || ndc1.x > 1) return;						 \n"
		"		if (ndc1.x < -1) ndc1 = ndc1 + vec * (-ndc1.x - 1);			 \n"
		"		if (ndc2.x > 1) ndc2 = ndc2 - vec * (ndc2.x - 1);			 \n"
		"																	 \n"
		"		if (ndc2.y < -1 || ndc1.y > 1) return;						 \n"
		"		if (ndc1.y < -1) ndc1 = ndc1 + vec * (-ndc1.y - 1);			 \n"
		"		if (ndc2.y > 1) ndc2 = ndc2 - vec * (ndc2.y - 1);			 \n"
		"																	 \n"
		"		if (ndc2.z < -1 || ndc1.z > 1) return;						 \n"
		"		if (ndc1.z < -1) ndc1 = ndc1 + vec * (-ndc1.z - 1);			 \n"
		"		if (ndc2.z > 1) ndc2 = ndc2 - vec * (ndc2.z - 1);			 \n"
		"	}																 \n"
		"																	 \n"
		"	vec2 sp1 = (ndc1.xy * .5f + .5f) * u_viewportSize;				 \n"
		"	vec2 sp2 = (ndc2.xy * .5f + .5f) * u_viewportSize;				 \n"
		"																	 \n"
		"	vec2 dir = normalize(sp2.xy - sp1.xy);							 \n"
		"	vec2 normal = vec2(-dir.y, dir.x);								 \n"
		"																	 \n"
		"	vec2 pos;														 \n"
		"																	 \n"
		"	pos = sp1 + (normal - dir) * geom_width[0];						 \n"
		"	frag_color = geom_color[0];										 \n"
		"	frag_pos = pos;													 \n"
		"	frag_pos1 = sp1.xy;												 \n"
		"	frag_pos2 = sp2.xy;												 \n"
		"	frag_width = geom_width[0];										 \n"
		"	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 \n"
		"	EmitVertex();													 \n"
		"																	 \n"
		"	pos = sp2 + (normal + dir) * geom_width[0];						 \n"
		"	frag_color = geom_color[0];										 \n"
		"	frag_pos = pos;													 \n"
		"	frag_pos1 = sp1.xy;												 \n"
		"	frag_pos2 = sp2.xy;												 \n"
		"	frag_width = geom_width[0];										 \n"
		"	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 \n"
		"	EmitVertex();													 \n"
		"																	 \n"
		"	pos = sp1 + (-normal + dir) * geom_width[0];					 \n"
		"	frag_color = geom_color[0];										 \n"
		"	frag_pos = pos;													 \n"
		"	frag_pos1 = sp1.xy;												 \n"
		"	frag_pos2 = sp2.xy;												 \n"
		"	frag_width = geom_width[0];										 \n"
		"	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 \n"
		"	EmitVertex();													 \n"
		"																	 \n"
		"	pos = sp2 + (-normal + dir) * geom_width[0];					 \n"
		"	frag_color = geom_color[0];										 \n"
		"	frag_pos = pos;													 \n"
		"	frag_pos1 = sp1.xy;												 \n"
		"	frag_pos2 = sp2.xy;												 \n"
		"	frag_width = geom_width[0];										 \n"
		"	gl_Position = vec4(vec2(pos / u_viewportSize * 2 - 1), 0, 1);	 \n"
		"	EmitVertex();													 \n"
		"																	 \n"
		"	EndPrimitive();													 \n"
		"}																	 \n";

		static const char* fragSource =
			"#version 450																\n"
			"																			\n"
			"in vec4 frag_color;														\n"
			"in vec2 frag_pos;															\n"
			"in vec2 frag_pos1;															\n"
			"in vec2 frag_pos2;															\n"
			"in float frag_width;														\n"
			"																			\n"
			"float dist(vec2 p1, vec2 p2, vec2 p)										\n"
			"{																			\n"
			"	const vec2 dir = p2 - p1;												\n"
			"	const float l2 = dir.x * dir.x + dir.y * dir.y;							\n"
			"																			\n"
			"	if (l2 == 0.0) return distance(p, p1);									\n"
			"																			\n"
			"	const float t = max(0, min(1, dot(p - p1, dir) / l2));					\n"
			"	const vec2 projection = p1 + t * dir;									\n"
			"	return distance(p, projection);											\n"
			"}																			\n"
			"																			\n"
			"void main()																\n"
			"{																			\n"
			"	float d = dist(frag_pos1, frag_pos2, frag_pos);							\n"
			"	d = (d - frag_width + 2) / 2; 											\n"
			"	d = clamp(d, 0, 1);														\n"
			"																			\n"
			"																			\n"
			"	if (d < 0.5f)															\n"
			"		gl_FragColor = vec4(frag_color.rgb, d < 0.5f ? frag_color.a : 0);	\n"
			"	else																	\n"
			"		discard;															\n"
			"																			\n"
			"	//gl_FragColor = frag_color;											\n"
			"}																			\n";

	struct Vertex
	{
		Vec3f pos1;
		Vec3f pos2;
		Vec4f color;		
		float width;	
	};

	Line3DRenderer::Line3DRenderer()
		: cache(nullptr), batchSize(0), batchOffset(0)
	{
		Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
		Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
		Core::FragmentShader frag; frag.ShaderSource(fragSource); frag.CompileShader();
		//Core::VertexShader vert { "assets/default/shaders/test.vert"}
		//Core::GeometryShader geom{ "assets/default/shaders/test.geom" };
		//Core::FragmentShader frag{ "assets/default/shaders/test.frag" };


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
		program.SetUniform(1, (Vec2f)Renderer::GetViewportSize());
	}

	void Line3DRenderer::SetBatchMode(uint batchSize)
	{
		delete[] cache;
		cache = new Vertex[batchSize];
		this->batchSize = batchSize;

		vb.AllocateDynamicStorage(BufferView(nullptr, batchSize * sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
	}
	void Line3DRenderer::SetImmediateMode()
	{
		delete[] cache;
		cache = nullptr;
		this->batchSize = 0;

		//vb.AllocateDynamicStorage(BufferView(nullptr, sizeof(Vertex)), Core::GraphicsBufferDynamicStorageHint::DynamicDraw);
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
			Vertex* vertices = (Vertex*)cache;
			vertices[batchOffset].pos1 = vertex.pos1;
			vertices[batchOffset].pos2 = vertex.pos2;
			vertices[batchOffset].color = vertex.color;
			vertices[batchOffset].width = vertex.width;

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
		if (batchOffset > 0)
		{
			vb.ChangeData(BufferView(cache, sizeof(Vertex) * batchOffset), 0);
			Renderer::SelectVertexArray(&va);
			Renderer::SelectProgram(&program);
			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, batchOffset);
		}
		batchOffset = 0;
	}
}