#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"
#include "BlazeEngine/Resources/Font/Font.h"

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
	static const char* fragSource[] = {
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
		"}													  \n",


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
		"														\n"
		"	if (color.rgb == vec3(0, 0, 0))					  \n"
		"		discard;									  \n"
		"	float l = (color.r * color.r + color.g * color.g + color.b * color.b) / 3; \n"
		"	l = pow(l, 0.5);\n"
		"	color = vec4(mix(color.rgb, u_color.rgb, l), 1);	   					  \n"		
		//"	color = vec4(color.rgb, u_color.a);	   					  \n"
		"													  \n"
		"	gl_FragColor = color;							  \n"
		"}													  \n",


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
		"	if (color.r < 0.45)								  \n"
		"		discard;									  \n"
		"	else if (color.r <= 0.50)						  \n"
		"		color = vec4(u_color.rgb, (color.r - 0.45) / 0.05 * u_color.a);\n"
		"	else		\n"
		"		color = u_color;							\n"
		//"	if (color.r > 0.5f)\n"
		//"		color = u_color; \n"
		//"	else discard; \n"
		"													  \n"
		"	gl_FragColor = color;							  \n"
		"}													  \n"
	};

	int GetRenderTypeProgramIndex(FontResolutionRenderType type)
	{
		switch (type)
		{
		case Blaze::FontResolutionRenderType::Monochrome: return 0;
		case Blaze::FontResolutionRenderType::Antialiased: return 0;
		case Blaze::FontResolutionRenderType::HorizontalLCD: return 1;
		case Blaze::FontResolutionRenderType::VerticalLCD: return 1;
		case Blaze::FontResolutionRenderType::SDF: return 2;
		}
	}

	TextShaderProgramSource GetDefaultTextShaderProgramSource_Normal()
	{
		return { vertSource, geomSource, fragSource[0] };
	}
	TextShaderProgramSource GetDefaultTextShaderProgramSource_LCD()
	{
		return { vertSource, geomSource, fragSource[1] };
	}
	TextShaderProgramSource GetDefaultTextShaderProgramSource_SDF()
	{
		return { vertSource, geomSource, fragSource[2] };
	}		

	using Vertex = TextRenderCache::Vertex;

	TextRenderer::TextRenderer()
		: fontResolution(nullptr)
	{		
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
	}
	TextRenderer::~TextRenderer()
	{
	}

	void TextRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
	}

	void TextRenderer::SetFontResolution(Blaze::FontResolution* resolution)
	{
		int programIndex = GetRenderTypeProgramIndex(resolution->GetRenderType());
		if (
			fontResolution == nullptr ||
			GetRenderTypeProgramIndex(fontResolution->GetRenderType()) != programIndex
			)
		{			
			Core::VertexShader vert; vert.ShaderSource(vertSource); vert.CompileShader();
			Core::GeometryShader geom; geom.ShaderSource(geomSource); geom.CompileShader();
			Core::FragmentShader frag; frag.ShaderSource(fragSource[programIndex]); frag.CompileShader();

			program.LinkShaders({ &vert, &geom, &frag });
		}

		fontResolution = resolution;
	}	

	Result TextRenderer::Write(const StringViewUTF8& text, Vec2f pos, ColorRGBAf color, float size)
	{
		DefaultTextVertexGenerator generator;
		return Write(text, pos, color, size, generator);
	}
	Result TextRenderer::Write(const StringViewUTF8& text, Vec2f pos, ColorRGBAf color, float size, BaseTextVertexGenerator& generator)
	{
		if (fontResolution == nullptr)
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
				"No font resolution set"));

		generator.Setup(text, fontResolution);

		Vertex* vertices = new Vertex[generator.GetMaxVertexCount()];
		Vertex* it = vertices;


		while (!generator.IsEnd())
			if (generator.GenerateVertex(it->p1, it->p2, it->uv1, it->uv2))
				++it;

		uint vertexCount = it - vertices;

		vb.AllocateDynamicStorage(
			BufferView(vertices, vertexCount * sizeof(Vertex)),
			Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);

		delete[] vertices;

		float scale = size / fontResolution->GetResolution();

		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&va);
		Renderer::SetActiveTextureSlot(0);
		Renderer::SelectTexture(&fontResolution->GetAtlas());
		program.SetUniform(0, proj);
		program.SetUniform(1, Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) * Math::ScalingMatrix(Vec3f(scale, scale, 1)));
		program.SetUniform(2, 0);
		program.SetUniform(3, (Vec4f)color);
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, vertexCount);

		return Result();
	}
	Result TextRenderer::Write(TextRenderCache& data, Vec2f pos, float size, ColorRGBAf color)
	{		
		if (fontResolution == nullptr)
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
				"No font resolution set"));

		if (GetRenderTypeProgramIndex(data.GetFontResolution()->GetRenderType()) !=
			GetRenderTypeProgramIndex(fontResolution->GetRenderType()))
			return Result(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
				"TextRenderData not compatible with the TextRenderer"));

		float scale = size / fontResolution->GetResolution();

		Renderer::SelectProgram(&program);
		Renderer::SelectVertexArray(&data.GetVertexArray());
		Renderer::SetActiveTextureSlot(0);
		Renderer::SelectTexture(&data.GetFontResolution()->GetAtlas());
		program.SetUniform(0, proj);
		program.SetUniform(1, Math::TranslationMatrix<float>(Vec3f(pos.x, pos.y, 0)) * Math::ScalingMatrix(Vec3f(scale, scale, 1)));
		program.SetUniform(2, 0);
		program.SetUniform(3, (Vec4f)color);
		Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, data.GetVertexCount());

		return Result();
	}	
}