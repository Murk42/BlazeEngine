#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

namespace Blaze
{
	namespace UI
	{
		Text::Text()
			: fontResolution(Graphics::GetDefaultFontResolution()), fontSize(Graphics::GetDefaultFontResolution()->GetResolution())
		{
			SetText("Text");

			va.EnableVertexAttribute(0);
			va.EnableVertexAttribute(1);
			va.EnableVertexAttribute(2);
			va.EnableVertexAttribute(3);
			va.EnableVertexAttribute(4);
			va.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
			va.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
			va.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
			va.SetVertexAttributeFormat(3, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
			va.SetVertexAttributeFormat(4, Graphics::Core::VertexAttributeType::Float, 4, false, sizeof(float) * 8);
			va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(4, &vb, sizeof(Vertex), 0);
		}

		Text::~Text()
		{
			vertices.clear();
		}

		void Text::SetFontSize(size_t fontSize)
		{
			this->fontSize = fontSize;
			if (fontResolution != nullptr)
				GenerateTextVertices();
		}

		void Text::SetFontResolution(FontResolution* fontResolution)
		{
			this->fontResolution = fontResolution;
			GenerateTextVertices();
		}

		void Text::SetText(StringUTF8 text)
		{
			this->text = text;
			if (fontResolution != nullptr)
				GenerateTextVertices();
		}

		void Text::GenerateTextVertices()
		{	
			Graphics::DefaultTextVertexGenerator generator;
			generator.Setup(text, fontResolution);

			vertices.clear();
			vertices.reserve(generator.GetMaxVertexCount());

			bottomLeft = { };
			topRight = { };

			float scale = (float)fontSize / fontResolution->GetResolution();

			while (!generator.IsEnd())
			{
				Text::Vertex vertex;

				if (generator.GenerateVertex(vertex.p1, vertex.p2, vertex.uv1, vertex.uv2, vertex.next))
				{
					vertex.p1 *= scale;
					vertex.p2 *= scale;
					vertex.next *= scale;

					bottomLeft.x = std::min({ bottomLeft.x, vertex.p1.x, vertex.p2.x });
					bottomLeft.y = std::min({ bottomLeft.y, vertex.p1.y, vertex.p2.y });
					topRight.x = std::max({ topRight.x, vertex.p1.x, vertex.p2.x });
					topRight.y = std::max({ topRight.y, vertex.p1.y, vertex.p2.y });

					vertices.emplace_back(vertex);
				}
			}			

			SetSize(Vec2f(topRight.x, std::round(fontSize * 0.6f)));

			vb.AllocateDynamicStorage(
				BufferView(vertices.data(), vertices.size() * sizeof(Text::Vertex)),
				Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw
			);
		}

		float Text::GetBaselineDistance() const
		{
			return (float)fontResolution->GetBaselineDistance() / fontResolution->GetResolution() * fontSize;
		}

		void Text::SetProperties(const TextProperties& p)
		{			
			if (p.text.edited) SetText(p.text);
			if (p.textColor.edited) color = p.textColor;

			if (p.fontSize.edited) SetFontSize(p.fontSize);
			if (p.fontResolution.edited) SetFontResolution(p.fontResolution);
		}

		Graphics::Core::ShaderProgram* TextManager::SelectProgram(FontResolutionRenderType renderType)
		{
			switch (renderType)
			{
			case Blaze::FontResolutionRenderType::Monochrome:
			case Blaze::FontResolutionRenderType::Antialiased:
				return &programNormal;
			case Blaze::FontResolutionRenderType::HorizontalLCD:
			case Blaze::FontResolutionRenderType::VerticalLCD:
				return &programLCD;
			case Blaze::FontResolutionRenderType::SDF:
				return &programSDF;
			default:
				return nullptr;
			}
		}

		void TextManager::Setup()
		{
			Graphics::Core::VertexShader vert{ "assets/default/shaders/ui_text.vert" };
			Graphics::Core::GeometryShader geom{ "assets/default/shaders/ui_text.geom" };
			Graphics::Core::FragmentShader fragNormal{ "assets/default/shaders/ui_text_normal.frag" };
			Graphics::Core::FragmentShader fragSDF{ "assets/default/shaders/ui_text_sdf.frag" };
			Graphics::Core::FragmentShader fragLCD{ "assets/default/shaders/ui_text_lcd.frag" };

			programNormal.LinkShaders({ &vert, &geom, &fragNormal });
			programSDF.LinkShaders({ &vert, &geom, &fragSDF });
			programLCD.LinkShaders({ &vert, &geom, &fragLCD });
		}

		void TextManager::Render(UIElement* element)
		{
			auto vp2d = GetManager()->GetProjectionMatrix();

			Text& text = (Text&)*element;

			if (text.fontResolution == nullptr)
			{
				Logger::AddLog(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
					"Text has its font resolution set to nullptr"));				
				return;
			}

			auto* program = SelectProgram(text.fontResolution->GetRenderType());
			auto* fontResolution = text.fontResolution;

			Graphics::Core::SelectProgram(program);

			if (!text.IsActive() || text.text.BufferSize() == 0)
				return;

			Vec2f alignedPos = text.GetViewportPos();			
			Rectf clipRect = text.GetClipRect();

			Graphics::Core::SelectVertexArray(&text.va);
			Graphics::Core::SetActiveTextureSlot(0);
			Graphics::Core::SelectTexture(&fontResolution->GetAtlas());
			program->SetUniform(0, Math::TranslationMatrix<float>(Vec3f(alignedPos, text.GetDepth())));
			program->SetUniform(1, GetManager()->GetProjectionMatrix());
			program->SetUniform(2, 0);
			program->SetUniform(3, (Vec4f)text.color);
			program->SetUniform(4, Vec4f(clipRect.pos, clipRect.size));

			Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Points, 0, text.vertices.size());
		}

		void TextManager::Update(UIElement* element)
		{
			Text& text = (Text&)*element;
		}
	}
}