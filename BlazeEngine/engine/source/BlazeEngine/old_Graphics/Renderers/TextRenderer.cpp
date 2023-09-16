#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Utility/TextVertexGenerator.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"

#include "GL/glew.h"


namespace Blaze::Graphics
{
	using Vertex = TextGraphicsData::Vertex;

	TextGraphicsData::TextGraphicsData()
		: vertices(nullptr), vertexCount(0), fontResolution(nullptr), fontHeight(0)
	{
	}
	TextGraphicsData::TextGraphicsData(TextGraphicsData&& data) noexcept
		: vertices(data.vertices), vertexCount(data.vertexCount), fontResolution(data.fontResolution), fontHeight(data.fontHeight), size(data.size)
	{
		data.vertices = nullptr;
		data.vertexCount = 0;
	}
	TextGraphicsData::TextGraphicsData(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, ArrayView<ColorRGBAf> colors, BaseTextVertexGenerator& vertexGenerator)
	{
		GenerateVertices(text, fontHeight, fontResolution, colors, vertexGenerator);
	}

	TextGraphicsData::~TextGraphicsData()
	{
		delete[] vertices;
	}

	Result TextGraphicsData::GenerateVertices(StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, ArrayView<ColorRGBAf> colors, BaseTextVertexGenerator& generator)
	{
		if (fontResolution == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "fontResolution is nullptr");

		uint maxVertexCount = generator.Setup(text, fontResolution);

		this->fontResolution = fontResolution;

		this->fontHeight = fontHeight;
		this->size = Vec2f();

		delete[] vertices;
		vertices = new Vertex[maxVertexCount];
		Vertex* it = vertices;

		float scale = fontHeight / fontResolution->GetResolution();

		uint index = 0;
		while (!generator.IsEnd())
		{
			
			if (generator.GetNextCharacterData(it->p1, it->p2))
			{
				it->p1 *= scale;
				it->p2 *= scale;

				size.x = std::max({ size.x, it->p1.x, it->p2.x });
				size.y = std::max({ size.y, -it->p1.y, -it->p2.y });

				if (index < colors.Count())
					it->color = (Vec4f)colors[index];
				else if (colors.Count() > 0)
					it->color = (Vec4f)colors.Last();
				else
					it->color = Vec4f(1, 1, 1, 1);

				++it;
				++index;
			}
		}

		//size.y += fontHeight * 0.6f;

		this->vertexCount = it - vertices;

		return Result();
	}

	void TextGraphicsData::SetColors(ArrayView<ColorRGBAf> colors)
	{
		for (int i = 0; i < vertexCount; ++i)
		{
			if (i < colors.Count())
				vertices[i].color = (Vec4f)colors[i];
			else if (colors.Count() > 0)
				vertices[i].color = (Vec4f)colors.Last();
			else
				vertices[i].color = Vec4f(1, 1, 1, 1);
		}
	}

	static constexpr size_t BatchCount = 4;
	static constexpr size_t BufferVertexCount = 128;
	static constexpr size_t BufferSize = BufferVertexCount * sizeof(Vertex);
	static constexpr size_t BatchVertexCount = BufferVertexCount / BatchCount;
	static constexpr size_t BatchSize = BatchVertexCount * sizeof(Vertex);

	TextRenderer::TextRenderer()
	{
		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeFormat(0, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, GraphicsLibrary::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeFormat(4, GraphicsLibrary::VertexAttributeType::Float, 4, false, sizeof(float) * 8);
		va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);

		renderer.Allocate(BatchCount, BatchSize);
	}
	TextRenderer::~TextRenderer()
	{
	}	

	bool FontResolutionRenderTypeToTextRenderingMethod(FontResolutionRenderType renderType, TextRenderer::TextRenderingMethod& method)
	{
		switch (renderType)
		{
		case Blaze::FontResolutionRenderType::Monochrome:
			method = TextRenderer::TextRenderingMethod::Normal;
			return true;
		case Blaze::FontResolutionRenderType::Antialiased:
			method = TextRenderer::TextRenderingMethod::Normal;
			return true;
		case Blaze::FontResolutionRenderType::HorizontalLCD:
			method = TextRenderer::TextRenderingMethod::LCD;
			return true;
		case Blaze::FontResolutionRenderType::SDF:
			method = TextRenderer::TextRenderingMethod::SDF;
			return true;
		default:
			return false;
		}
	}

	void TextRenderer::SetProjectionMatrix(Mat4f mat)
	{
		proj = mat;
	}
	
	Result TextRenderer::Render(Transform2D transform, const TextGraphicsData& data) const
	{		
		return Render(transform.GetMatrix(data.GetSize()), data);
	}
	Result TextRenderer::Render(Mat4f matrix, const TextGraphicsData& data) const
	{
		auto* fontResolution = data.GetFontResolution();
		TextRenderingMethod method;

		if (fontResolution == nullptr)
			return BLAZE_ERROR_RESULT("Blaze Engine", "Set font resolution is nullptr");

		if (!FontResolutionRenderTypeToTextRenderingMethod(fontResolution->GetRenderType(), method))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Unsuported FontResolutionRenderType. The enum int value was: " + StringParsing::Convert((int)fontResolution->GetRenderType()));

		GraphicsLibrary::ShaderProgram* program = nullptr;

		CHECK_RESULT(CheckProgram(method, program));			

		Graphics::Renderer::SelectProgram(program);

		Graphics::Renderer::SelectVertexArray(&va);
		Graphics::Renderer::SetActiveTextureSlot(0);
		Graphics::Renderer::SelectTexture(&fontResolution->GetAtlas());
		program->SetUniform(0, matrix);
		program->SetUniform(1, proj);
		program->SetUniform(2, 0);

		if (method == TextRenderingMethod::LCD)
			glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);

		
		auto vertices = data.GetVertices();

		CHECK_RESULT(renderer.Render(vertices, [](uint startVertex, uint vertexCount) {			
			return Graphics::Renderer::RenderPrimitiveArray(Graphics::Renderer::PrimitiveType::Points, startVertex, vertexCount);			
			}));		

		if (method == TextRenderingMethod::LCD)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return Result();
	}
	Result TextRenderer::SetShaderProgram(GraphicsLibrary::ShaderProgram&& program, TextRenderingMethod methodType)
	{
		GraphicsLibrary::ShaderProgram* ptr = nullptr;

		if (!GetProgram(methodType, ptr))
			return BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextRenderingMethod enum, int value was: " + StringParsing::Convert((int)methodType));		

		*ptr = std::move(program);
		return Result();
	}
	Result TextRenderer::LoadDefaultShaderProgram(TextRenderingMethod methodType) const
	{
		const char* fragPath;
		GraphicsLibrary::ShaderProgram* program;

		switch (methodType)
		{
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::Normal:
			fragPath = "assets/default/shaders/text/text_normal.frag";
			program = &programNormal;
			break;
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::SDF:
			fragPath = "assets/default/shaders/text/text_sdf.frag";
			program = &programSDF;
			break;
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::LCD:
			fragPath = "assets/default/shaders/text/text_lcd.frag";
			program = &programLCD;
			break;
		default:
			return BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextRenderingMethod enum, int value was: " + StringParsing::Convert((int)methodType));
		}

		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader frag;

		CHECK_RESULT(vert.Load("assets/default/shaders/text/text.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/text/text.geom"));
		CHECK_RESULT(frag.Load(fragPath));

		if (program->GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
			*program = std::move(GraphicsLibrary::ShaderProgram());
		return program->LinkShaders({ &vert, &geom, &frag });
	}
	Result TextRenderer::LoadDefaultShaderPrograms() const
	{
		GraphicsLibrary::VertexShader vert;
		GraphicsLibrary::GeometryShader geom;
		GraphicsLibrary::FragmentShader fragNormal;
		GraphicsLibrary::FragmentShader fragSDF;
		GraphicsLibrary::FragmentShader fragLCD;
		
		CHECK_RESULT(vert.Load("assets/default/shaders/text/text.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/text/text.geom"));
		CHECK_RESULT(fragNormal.Load("assets/default/shaders/text/text_norm"));
		CHECK_RESULT(fragSDF.Load("assets/default/shaders/text/text_sdf.frag"));
		CHECK_RESULT(fragLCD.Load("assets/default/shaders/text/text_lcd.frag"));

		if (fragNormal.GetState() == GraphicsLibrary::ShaderState::SuccesfullyCompiled)
		{
			if (programNormal.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
				programNormal = std::move(GraphicsLibrary::ShaderProgram());
			CHECK_RESULT(programNormal.LinkShaders({ &vert, &geom, &fragNormal }));
		}

		if (fragSDF.GetState() == GraphicsLibrary::ShaderState::SuccesfullyCompiled)
		{
			if (programSDF.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
				programSDF = std::move(GraphicsLibrary::ShaderProgram());
			CHECK_RESULT(programSDF.LinkShaders({ &vert, &geom, &fragSDF }));
		}

		if (fragLCD.GetState() == GraphicsLibrary::ShaderState::SuccesfullyCompiled)
		{
			if (programLCD.GetState() != GraphicsLibrary::ShaderProgramState::Invalid)
				programLCD = std::move(GraphicsLibrary::ShaderProgram());
			CHECK_RESULT(programLCD.LinkShaders({ &vert, &geom, &fragLCD }));
		}

		return Result();
	}

	Result TextRenderer::CheckProgram(TextRenderingMethod method, GraphicsLibrary::ShaderProgram*& program) const
	{
		CHECK_RESULT(GetProgram(method, program));

		switch (program->GetState())
		{
		case GraphicsLibrary::ShaderProgramState::Invalid:
			CHECK_RESULT(LoadDefaultShaderProgram(method));
			break;
		case GraphicsLibrary::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to write text but the shader program wasn't succesfully linked");
		case GraphicsLibrary::ShaderProgramState::SuccesfullyLinked:
			break;
		}

		return Result();
	}	

	Result TextRenderer::GetProgram(TextRenderingMethod method, GraphicsLibrary::ShaderProgram*& program) const
	{
		switch (method)
		{
		case TextRenderingMethod::Normal:
			program = &programNormal;
			break;
		case TextRenderingMethod::LCD:
			program = &programLCD;
			break;
		case TextRenderingMethod::SDF:
			program = &programSDF;
			break;
		default:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Invalid TextRenderingMethod enum value");
		}
	
		return Result();
	}
}