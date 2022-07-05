#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze::Graphics
{
	static const char* vertSource =
		"#version 450 core					   \n"
		"									   \n"
		"layout(location = 0) in vec2 i_pos1;  \n"
		"layout(location = 1) in vec2 i_pos2;  \n"
		"layout(location = 2) in vec2 i_uv1;   \n"
		"layout(location = 3) in vec2 i_uv2;   \n"
		"									   \n"
		"out vec2 geom_pos1;				   \n"
		"out vec2 geom_pos2;				   \n"
		"out vec2 geom_uv1;					   \n"
		"out vec2 geom_uv2;					   \n"
		"									   \n"
		"void main()						   \n"
		"{									   \n"
		"	geom_pos1 = i_pos1;				   \n"
		"	geom_pos2 = i_pos2;				   \n"
		"	geom_uv1 = i_uv1;				   \n"
		"	geom_uv2 = i_uv2;				   \n"
		"}									   \n";

	static const char* geomSource =
		"#version 450															  \n"
		"																		  \n"
		"layout(points) in;														  \n"
		"layout(triangle_strip, max_vertices = 4) out;							  \n"
		"																		  \n"
		"in vec2 geom_pos1[];													  \n"
		"in vec2 geom_pos2[];													  \n"
		"in vec2 geom_uv1[];													  \n"
		"in vec2 geom_uv2[];													  \n"
		"																		  \n"
		"out vec2 frag_uv;														  \n"
		"																		  \n"
		"layout(location = 0) uniform mat4 u_proj;								  \n"
		"layout(location = 1) uniform mat4 u_VP;								  \n"
		"																		  \n"
		"void main()															  \n"
		"{																		  \n"
		"	gl_Position = u_proj * u_VP * vec4(geom_pos1[0].x, geom_pos1[0].y, 1, 1);	  \n"
		"	frag_uv = vec2(geom_uv1[0].x, geom_uv1[0].y);						  \n"
		"	EmitVertex();														  \n"
		"																		  \n"
		"	gl_Position = u_proj * u_VP * vec4(geom_pos1[0].x, geom_pos2[0].y, 1, 1);	  \n"
		"	frag_uv = vec2(geom_uv1[0].x, geom_uv2[0].y);						  \n"
		"	EmitVertex();														  \n"
		"																		  \n"
		"	gl_Position = u_proj * u_VP * vec4(geom_pos2[0].x, geom_pos1[0].y, 1, 1);	  \n"
		"	frag_uv = vec2(geom_uv2[0].x, geom_uv1[0].y);						  \n"
		"	EmitVertex();														  \n"
		"																		  \n"
		"	gl_Position = u_proj * u_VP * vec4(geom_pos2[0].x, geom_pos2[0].y, 1, 1);	  \n"
		"	frag_uv = vec2(geom_uv2[0].x, geom_uv2[0].y);						  \n"
		"	EmitVertex();														  \n"
		"																		  \n"
		"	EndPrimitive();														  \n"
		"}																		  \n";
	static const char* fragSource =
		"#version 450										  \n"
		"													  \n"
		"in vec2 frag_uv;									  \n"
		"													  \n"
		"layout(location = 2) uniform sampler2D u_texture;	  \n"
		"layout(location = 3) uniform vec4 u_color;			  \n"
		"													  \n"
		"void main()										  \n"
		"{													  \n"
		"	vec4 color = texture(u_texture, frag_uv);		  \n"
		"													  \n"
		"	color = vec4(1, 1, 1, color.r) * u_color;		  \n"
		"	if (color.a == 0)								  \n"
		"		discard;									  \n"
		"													  \n"
		"	gl_FragColor = color;							  \n"
		"}													  \n";

	TextRenderer::TextRenderer()
	{
		Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
		Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
		Core::FragmentShader frag; frag.ShaderSource(fragSource); frag.CompileShader();

		program.LinkShaders({ &vert, &geom, &frag });

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Font::CharacterVertex), 0);
	}
	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::SetProjectionMatrix(Mat4f mat)
	{
		program.SetUniform(0, mat);
	}

	void TextRenderer::SetFont(Font* font)
	{
		this->font = font;
	}

	void TextRenderer::Write(const StringViewUTF8& text, float height, Vec2i pos, ColorRGBA color)
	{
		uint vertexCount;
		Vec2f size;
		Vec2f bottomLeft;
		Vec2f topRight;
		Font::CharacterVertex* vertices = font->GenerateVertices(text, vertexCount, size, bottomLeft, topRight);
		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(Font::CharacterVertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);				

		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&va);
		Renderer::SetActiveTextureSlot(0);
		Renderer::SelectTexture(&font->GetTexture());
		program.SetUniform(1,
			Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) *
			Math::ScalingMatrix<float>(Vec3f(height, height, 0))
		);
		program.SetUniform(2, 0);
		program.SetUniform(3, ColorRGBAf(color).ToVector());
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, vertexCount);
	}

	void TextRenderer::Write(TextRenderData& data, Vec2i pos, ColorRGBA color)
	{		
		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&data.va);
		Renderer::SetActiveTextureSlot(0);
		Renderer::SelectTexture(&font->GetTexture());
		program.SetUniform(1,
			Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) *
			Math::ScalingMatrix<float>(Vec3f(data.height, data.height, 0))
		);
		program.SetUniform(2, 0);
		program.SetUniform(3, ColorRGBAf(color).ToVector());
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, data.vertexCount);
	}

	TextRenderData::TextRenderData(const TextRenderer& renderer, StringViewUTF8 text, float height)
	{				
		this->renderer = &renderer;
		this->height = height;
		Font::CharacterVertex* vertices = renderer.font->GenerateVertices(text, vertexCount, size, bottomLeft, topRight);
		bottomLeft *= height;
		topRight *= height;
		size *= height;
		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(Font::CharacterVertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Font::CharacterVertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Font::CharacterVertex), 0);
	}


	TextRenderData::TextRenderData(TextRenderData&& data)
		: vb(std::move(data.vb)), va(std::move(data.va)), vertexCount(data.vertexCount), size(data.size), renderer(data.renderer), height(data.height)
	{
	}


}