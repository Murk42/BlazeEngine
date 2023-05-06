#include "BlazeEngine/Graphics/Renderers/TextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"
#include "BlazeEngine/Graphics/Renderers/TextRenderCache.h"

#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"

#include "GL/glew.h"

namespace Blaze::Graphics
{
	using Vertex = TextRenderCache::Vertex;
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
		va.SetVertexAttributeFormat(0, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
		va.SetVertexAttributeFormat(1, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
		va.SetVertexAttributeFormat(2, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
		va.SetVertexAttributeFormat(3, Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
		va.SetVertexAttributeFormat(4, Core::VertexAttributeType::Float, 4, false, sizeof(float) * 8);
		//va.SetVertexAttributeBuffer(0, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(1, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(2, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(3, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//va.SetVertexAttributeBuffer(4, &renderer.GetBuffer(), sizeof(Vertex), 0);
		//
		//renderer.Allocate(BatchCount, BatchSize);
		//vb.Allocate(nullptr, BufferSize,
		//	Graphics::Core::ImmutableGraphicsBufferMapAccess::Write,
		//	Graphics::Core::ImmutableGraphicsBufferMapType::PersistentCoherent
		//);
		//vbMap = vb.MapBufferRange(0, BufferSize,			
		//	Graphics::Core::ImmutableGraphicsBufferMapOptions::InvalidateRange
		//);
	}
	TextRenderer::~TextRenderer()
	{
	}

	bool TextRenderer::GetProgram(TextRenderingMethod method, Graphics::Core::ShaderProgram*& program)
	{
		switch (method)
		{
		case TextRenderingMethod::Normal:
			program = &programNormal;
			return true;
		case TextRenderingMethod::LCD:
			program = &programLCD;
			return true;
		case TextRenderingMethod::SDF:
			program = &programSDF;
			return true;
		default:
			return false;
		}
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

	Result TextRenderer::Write(Vec2f pos, StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors)
	{
		DefaultTextVertexGenerator generator;
		return Write(pos, text, fontHeight, fontResolution, colors, generator);
	}	
	Result TextRenderer::Write(Vec2f pos, StringViewUTF8 text, float fontHeight, FontResolution* fontResolution, std::span<const ColorRGBAf> colors, BaseTextVertexGenerator& generator)
	{
		if (fontResolution == nullptr)
			return BLAZE_WARNING_RESULT("BlazeEngine", "No font resolution set");

		TextRenderCache cache;
		cache.GenerateVertices(text, fontHeight, fontResolution, colors, generator);		
		Write(pos, cache);

		return Result();
	}	
	Result TextRenderer::Write(Vec2f pos, TextRenderCache& data)
	{
		auto* fontResolution = data.GetFontResolution();
		TextRenderingMethod method;

		if (!FontResolutionRenderTypeToTextRenderingMethod(fontResolution->GetRenderType(), method))
			return BLAZE_ERROR_RESULT("Blaze Engine", "Unsuported FontResolutionRenderType. The enum int value was: " + StringParsing::Convert((int)fontResolution->GetRenderType()));

		Graphics::Core::ShaderProgram* program = nullptr;

		if (!GetProgram(method, program))
			return BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextRenderingMethod enum, int value was: " + StringParsing::Convert((int)method));

		switch (program->GetState())
		{
		case Graphics::Core::ShaderProgramState::Invalid:
			if (Result r = LoadDefaultShaders(method)) return r;
			break;
		case Graphics::Core::ShaderProgramState::UnsuccesfullyLinked:
			return BLAZE_ERROR_RESULT("Blaze Engine", "Trying to write text but the shader program wasn't succesfully linked");
		case Graphics::Core::ShaderProgramState::SuccesfullyLinked:
			break;
		}

		Graphics::Core::SelectProgram(program);

		Graphics::Core::SelectVertexArray(&va);
		Graphics::Core::SetActiveTextureSlot(0);
		Graphics::Core::SelectTexture(&fontResolution->GetAtlas());
		program->SetUniform(0, pos);
		program->SetUniform(1, proj);
		program->SetUniform(2, 0);

		if (method == TextRenderingMethod::LCD)
			glBlendFuncSeparate(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);

		
		auto vertices = data.GetVertices();

		//renderer.Render(vertices.data(), vertices.size(), [](uint startVertex, uint vertexCount) {			
		//	Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Points, startVertex, vertexCount);
		//	});

		//while (true)
		//{
		//	uint vertexCount = std::min<uint>(vertices.size() - offset, BatchVertexCount);
		//
		//	vb.WriteData(vertices.data() + offset, vertexCount * sizeof(Vertex));			
		//
		//
		//	vb.NextBatch();			
		//
		//	offset += vertexCount;
		//
		//	if (offset == vertices.size())
		//		break;			
		//}

		if (method == TextRenderingMethod::LCD)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		return Result();
	}
	Result TextRenderer::SetShaders(std::initializer_list<Graphics::Core::Shader*> shaders, TextRenderingMethod methodType)
	{
		Graphics::Core::ShaderProgram* program = nullptr;

		if (!GetProgram(methodType, program))
			return BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextRenderingMethod enum, int value was: " + StringParsing::Convert((int)methodType));		

		if (program->GetState() != Graphics::Core::ShaderProgramState::Invalid)
			*program = std::move(Graphics::Core::ShaderProgram());

		return program->LinkShaders(shaders);
	}
	Result TextRenderer::LoadDefaultShaders(TextRenderingMethod methodType)
	{
		const char* fragPath;
		Graphics::Core::ShaderProgram* program;

		switch (methodType)
		{
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::Normal:
			fragPath = "assets/default/shaders/ui_text_normal.frag";
			program = &programNormal;
			break;
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::SDF:
			fragPath = "assets/default/shaders/ui_text_sdf.frag";
			program = &programSDF;
			break;
		case Blaze::Graphics::TextRenderer::TextRenderingMethod::LCD:
			fragPath = "assets/default/shaders/ui_text_lcd.frag";
			program = &programLCD;
			break;
		default:
			return BLAZE_ERROR_LOG("Blaze Engine", "Invalid TextRenderingMethod enum, int value was: " + StringParsing::Convert((int)methodType));
		}

		Graphics::Core::VertexShader vert;
		Graphics::Core::GeometryShader geom;
		Graphics::Core::FragmentShader frag;

		if (Result r = vert.Load("assets/default/shaders/ui_text.vert")) return r;
		if (Result r = geom.Load("assets/default/shaders/ui_text.geom")) return r;
		if (Result r = frag.Load(fragPath)) return r;

		if (program->GetState() != Graphics::Core::ShaderProgramState::Invalid)
			*program = std::move(Graphics::Core::ShaderProgram());
		return program->LinkShaders({ &vert, &geom, &frag });
	}
	Result TextRenderer::LoadDefaultShaders()
	{
		Graphics::Core::VertexShader vert;
		Graphics::Core::GeometryShader geom;
		Graphics::Core::FragmentShader fragNormal;
		Graphics::Core::FragmentShader fragSDF;
		Graphics::Core::FragmentShader fragLCD;
		
		CHECK_RESULT(vert.Load("assets/default/shaders/ui_text.vert"));
		CHECK_RESULT(geom.Load("assets/default/shaders/ui_text.geom"));
		CHECK_RESULT(fragNormal.Load("assets/default/shaders/ui_text_norm"));
		CHECK_RESULT(fragSDF.Load("assets/default/shaders/ui_text_sdf.frag"));
		CHECK_RESULT(fragLCD.Load("assets/default/shaders/ui_text_lcd.frag"));

		if (fragNormal.GetState() == Graphics::Core::ShaderState::SuccesfullyCompiled)
		{
			if (programNormal.GetState() != Graphics::Core::ShaderProgramState::Invalid)
				programNormal = std::move(Graphics::Core::ShaderProgram());
			CHECK_RESULT(programNormal.LinkShaders({ &vert, &geom, &fragNormal }));
		}

		if (fragSDF.GetState() == Graphics::Core::ShaderState::SuccesfullyCompiled)
		{
			if (programSDF.GetState() != Graphics::Core::ShaderProgramState::Invalid)
				programSDF = std::move(Graphics::Core::ShaderProgram());
			CHECK_RESULT(programSDF.LinkShaders({ &vert, &geom, &fragSDF }));
		}

		if (fragLCD.GetState() == Graphics::Core::ShaderState::SuccesfullyCompiled)
		{
			if (programLCD.GetState() != Graphics::Core::ShaderProgramState::Invalid)
				programLCD = std::move(Graphics::Core::ShaderProgram());
			CHECK_RESULT(programLCD.LinkShaders({ &vert, &geom, &fragLCD }));
		}

		return Result();
	}
}