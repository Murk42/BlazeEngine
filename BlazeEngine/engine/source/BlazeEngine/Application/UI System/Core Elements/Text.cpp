#include "BlazeEngine/Application/UI System/Core Elements/Text.h"
#include "BlazeEngine/Graphics/Renderer.h"
#include "BlazeEngine/Math/Math.h"

#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

namespace Blaze
{
	namespace UI
	{
		struct Vertex
		{
			Vec2f p1;
			Vec2f p2;
			Vec2f uv1;
			Vec2f uv2;
		};

		Text::Text()
			: text("Text!"),
			color(1, 1, 1, 1), shown(true), dirty(true),
			vertexCount(0), clipRect(0, 0, 0, 0), fontSize(20.0f), fontResolution(Graphics::GetDefaultFontResolution())
		{
			va.EnableVertexAttribute(0);
			va.EnableVertexAttribute(1);
			va.EnableVertexAttribute(2);
			va.EnableVertexAttribute(3);
			va.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 0);
			va.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2);
			va.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4);
			va.SetVertexAttributeFormat(3, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6);
			va.SetVertexAttributeBuffer(0, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(1, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(2, &vb, sizeof(Vertex), 0);
			va.SetVertexAttributeBuffer(3, &vb, sizeof(Vertex), 0);		
		}				

		void Text::SetFontResolution(FontResolution* fontResolution)
		{
			this->fontResolution = fontResolution;
			dirty = true;
		}

		void Text::SetText(StringViewUTF8 text)
		{
			this->text = text;
			dirty = true;
		}		

		void TextManager::GenerateVertices(Text& text, FontResolution* fontResolution)
		{			
			text.dirty = false;
			text.vertexCount = 0;

			Graphics::DefaultTextVertexGenerator generator;
			generator.Setup(text.text, fontResolution);

			Vertex* vertices = new Vertex[generator.GetMaxVertexCount()];
			Vertex* it = vertices;

			while (!generator.IsEnd())
			{
				if (generator.GenerateVertex(it->p1, it->p2, it->uv1, it->uv2))
				{
					text.bottomLeft.x = std::min({ text.bottomLeft.x, it->p1.x, it->p2.x });
					text.bottomLeft.y = std::min({ text.bottomLeft.y, it->p1.y, it->p2.y });
					text.topRight.x = std::max({ text.topRight.x, it->p1.x, it->p2.x });
					text.topRight.y = std::max({ text.topRight.y, it->p1.y, it->p2.y });					

					++it;
					++text.vertexCount;
				}
			}		

			text.bottomLeft *= text.fontSize / fontResolution->GetResolution();
			text.topRight *= text.fontSize / fontResolution->GetResolution();

			text.SetSize(text.topRight);

			text.vb.AllocateDynamicStorage(
				BufferView(vertices, text.vertexCount * sizeof(Vertex)),
				Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw
			);

			delete[] vertices;
		}

		Graphics::Core::ShaderProgram* TextManager::SelectProgram(Text& text)
		{			
			switch (text.fontResolution->GetRenderType())
			{			
			case Blaze::FontResolutionRenderType::Monochrome:				
			case Blaze::FontResolutionRenderType::Antialiased:
				return &programNormal;
			case Blaze::FontResolutionRenderType::HorizontalLCD:			
			case Blaze::FontResolutionRenderType::VerticalLCD:
				return &programLCD;
			case Blaze::FontResolutionRenderType::SDF:
				return &programSDF;
			}
		}

		void TextManager::Setup()
		{			
			Graphics::Core::VertexShader vert			{ "assets/default/shaders/ui_text.vert" };
			Graphics::Core::GeometryShader geom			{ "assets/default/shaders/ui_text.geom" };
			Graphics::Core::FragmentShader fragNormal	{ "assets/default/shaders/ui_text_normal.frag" };
			Graphics::Core::FragmentShader fragSDF		{ "assets/default/shaders/ui_text_sdf.frag" };
			Graphics::Core::FragmentShader fragLCD		{ "assets/default/shaders/ui_text_lcd.frag" };

			programNormal.LinkShaders({ &vert, &geom, &fragNormal });
			programSDF.LinkShaders({ &vert, &geom, &fragSDF });
			programLCD.LinkShaders({ &vert, &geom, &fragLCD });
		}
		
		void TextManager::Render(size_t index, size_t end)
		{			
			Vec2i size = Renderer::GetViewportSize();
			Mat4f vp2d = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1000, 1000);

			for (; index != end; ++index)
			{
				Text& text = *GetElement(index);

				if (text.fontResolution == nullptr)
				{
					Logger::AddLog(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
						"Text has its font resolution set to nullptr"));
					continue;
				}
								
				auto* program = SelectProgram(text);
				auto* fontResolution = text.fontResolution;

				Renderer::SelectProgram(program);

				if (text.shown)
				{					
					Vec2f alignedPos = text.GetAlignedPos();
										
					float scale = text.fontSize / fontResolution->GetResolution();

					Renderer::SelectVertexArray(&text.va);
					Renderer::SetActiveTextureSlot(0);
					Renderer::SelectTexture(&fontResolution->GetAtlas());
					program->SetUniform(0, vp2d * Math::TranslationMatrix<float>(Vec3f(alignedPos, text.GetDepth())) * Math::ScalingMatrix(Vec3f(scale, scale, 1)));
					program->SetUniform(1, Vec2f(1, 1));					
					program->SetUniform(2, 0);
					program->SetUniform(3, (Vec4f)text.color);
					program->SetUniform(4
						, Vec4f(text.clipRect.pos, text.clipRect.pos + text.clipRect.size));

					Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Points, 0, text.vertexCount);
				}
			}
		}

		void TextManager::Update(size_t index, size_t end)
		{
			for (; index != end; ++index)
			{
				Text& text = *GetElement(index);

				if (text.dirty)
				{								
					if (text.fontResolution == nullptr)
					{
						Logger::AddLog(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
							"Text has its font resolution set to nullptr"));
						continue;
					}					
					

					GenerateVertices(text, text.fontResolution);

					if (text.sizeChanged)
						text.sizeChanged();
				}
			}
		}
	}
}