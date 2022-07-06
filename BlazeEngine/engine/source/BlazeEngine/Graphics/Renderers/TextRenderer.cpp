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

	struct CharacterVertex
	{
		Vec2f p1;
		Vec2f p2;
		Vec2f uv1;
		Vec2f uv2;
	};

	FontResolution* TextRenderer::SelectResolution(int res) const
	{
		if (fontResolutions.size() == 0)
			return 0;

		FontResolution* upper = nullptr;
		FontResolution* lower = nullptr; 

		for (auto& fontRes : fontResolutions)
		{
			int currRes = fontRes->GetResolution();

			if (currRes == res)
				return fontRes;
			else if (currRes < res && currRes > lower->GetResolution())
				lower = fontRes;
			else if (currRes > res && currRes < upper->GetResolution())
				upper = fontRes;			
		}		

		if (upper == nullptr)
			return lower;

		return upper;
	}

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
		va.SetVertexAttributeBuffer(0, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(CharacterVertex), 0);
	}
	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::SetProjectionMatrix(Mat4f mat)
	{
		program.SetUniform(0, mat);
	}

	void TextRenderer::SetResolutions(std::initializer_list<FontResolution*> resolutions)
	{
		fontResolutions = resolutions;
	}	

	void TextRenderer::Write(const StringViewUTF8& text, int resolution, Vec2f pos, ColorRGBA color)
	{
		FontResolution* res = SelectResolution(resolution);
		
		uint vertexCount = text.CharacterCount();
		CharacterVertex* vertices = new CharacterVertex[vertexCount];

		unsigned i = 0;
		for (auto it = text.begin(); it != text.end(); ++it, ++i)
		{
			UnicodeChar ch = it.ToUnicode();
			CharacterData data = res->GetCharacterData(ch);
	
			vertices[i].p1 = pos + data.renderOffset;
			vertices[i].p2 = vertices[i].p1 + data.size;
			vertices[i].uv1 = data.uv1;
			vertices[i].uv2 = data.uv2;			
		}

		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(CharacterVertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);				

		delete[] vertices;

		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&va);
		Renderer::SetActiveTextureSlot(0);
		Renderer::SelectTexture(&res->GetAtlas());
		program.SetUniform(1,
			Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) *
			Math::ScalingMatrix<float>(Vec3f(resolution, resolution, 0))
		);
		program.SetUniform(2, 0);
		program.SetUniform(3, ColorRGBAf(color).ToVector());
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, vertexCount);
	}

	void TextRenderer::Write(TextRenderData& data, Vec2f pos, ColorRGBA color)
	{		
		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&data.va);
		Renderer::SetActiveTextureSlot(0
		);
		Renderer::SelectTexture(&data.fontResolution->GetAtlas());
		program.SetUniform(1,
			Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0))
		);
		program.SetUniform(2, 0);
		program.SetUniform(3, ColorRGBAf(color).ToVector());
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, data.vertexCount);
	}

	TextRenderData::TextRenderData(const TextRenderer& renderer, StringViewUTF8 text, float resolution)
	{				
		this->renderer = &renderer;

		bottomLeft = { FLT_MAX, FLT_MAX };
		topRight = { FLT_MIN, FLT_MIN };

		fontResolution = renderer.SelectResolution(resolution);
		float res = fontResolution->GetResolution();

		uint vertexCount = text.CharacterCount();
		CharacterVertex* vertices = new CharacterVertex[vertexCount];

		unsigned i = 0;
		for (auto it = text.begin(); it != text.end(); ++it, ++i)
		{
			UnicodeChar ch = it.ToUnicode();
			CharacterData data = fontResolution->GetCharacterData(ch);

			vertices[i].p1 = data.renderOffset;
			vertices[i].p2 = vertices[i].p1 + data.renderOffset + data.size;
			vertices[i].uv1 = data.uv1;
			vertices[i].uv2 = data.uv2;

			bottomLeft.x = std::min(bottomLeft.x, vertices[i].p1.x);
			bottomLeft.y = std::min(bottomLeft.y, vertices[i].p1.y);
			topRight.x = std::max(topRight.x, vertices[i].p2.x);
			topRight.y = std::max(topRight.y, vertices[i].p2.y);
		}

		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(CharacterVertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);

		bottomLeft *= fontResolution->GetResolution();
		topRight *= fontResolution->GetResolution();
		size *= fontResolution->GetResolution();
		delete[] vertices;

		//CharacterVertex* vertices = renderer.font->GenerateVertices(text, vertexCount, size, bottomLeft, topRight);
		//vb.AllocateDynamicStorage(
		//	BufferView(vertices, vertexCount * sizeof(Font::CharacterVertex)),
		//	Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		//);

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(CharacterVertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(CharacterVertex), 0);
	}


	TextRenderData::TextRenderData(TextRenderData&& data)
		: vb(std::move(data.vb)), va(std::move(data.va)), vertexCount(data.vertexCount), size(data.size), renderer(data.renderer), fontResolution(data.fontResolution)
	{
	}


}