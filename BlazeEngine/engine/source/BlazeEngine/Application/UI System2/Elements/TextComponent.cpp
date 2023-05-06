#include "BlazeEngine/Application/UI System2/Components/TextComponent.h"
#include "BlazeEngine/Application/UI System2/Components/UIElementComponent.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"

namespace Blaze::UI2::Components
{
	TextComponent::TextComponent(ECS::ComponentConstructData data)
		: Component(data), fontResolution(Graphics::GetDefaultFontResolution()), fontSize(Graphics::GetDefaultFontResolution()->GetResolution())
	{
		uiElementComponent = GetEntity()->GetComponent<UIElementComponent>();

		SetText("Text");

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

	TextComponent::~TextComponent()
	{
		vertices.clear();
	}

	void TextComponent::SetFontSize(size_t fontSize)
	{
		this->fontSize = fontSize;
		if (fontResolution != nullptr)
			GenerateTextVertices();
	}

	void TextComponent::SetFontResolution(FontResolution* fontResolution)
	{
		this->fontResolution = fontResolution;
		GenerateTextVertices();
	}

	void TextComponent::SetText(StringUTF8 text)
	{
		this->text = text;
		if (fontResolution != nullptr)
			GenerateTextVertices();
	}

	void TextComponent::GenerateTextVertices()
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
			Vertex vertex;

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

		uiElementComponent->SetSize(Vec2f(topRight.x, std::round(fontSize * 0.6f)));

		vb.AllocateDynamicStorage(
			BufferView(vertices.data(), vertices.size() * sizeof(Vertex)),
			Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw
		);
	}

	float TextComponent::GetBaselineDistance() const
	{
		return (float)fontResolution->GetBaselineDistance() / fontResolution->GetResolution() * fontSize;
	}


	namespace ComponentSystems
	{
		Graphics::Core::ShaderProgram* TextSystem::SelectProgram(FontResolutionRenderType renderType)
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

		TextSystem::TextSystem()
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

		void TextSystem::Created(ECS::Component* component)
		{			
		}
		void TextSystem::Render(ECS::Component* component)
		{
			TextComponent& text = *component->Cast<TextComponent>();
			UIElementComponent& element = *text.uiElementComponent;

			if (text.fontResolution == nullptr)
			{
				Logger::AddLog(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
					"Text has its font resolution set to nullptr"));
				Graphics::GetPoint2DRenderer().SetImmediateMode();
				Graphics::DrawPoint2D(element.GetViewportPos() + element.GetSize() / 2, 0xff0000ff, 10);
				return;
			}

			auto* program = SelectProgram(text.fontResolution->GetRenderType());
			auto* fontResolution = text.fontResolution;

			Graphics::Core::SelectProgram(program);

			if (!element.IsActive() || text.text.BufferSize() == 0)
				return;

			Vec2f alignedPos = element.GetViewportPos();
			Rectf clipRect = element.GetClipRect();

			Graphics::Core::SelectVertexArray(&text.va);
			Graphics::Core::SetActiveTextureSlot(0);
			Graphics::Core::SelectTexture(&fontResolution->GetAtlas());
			program->SetUniform(0, Math::TranslationMatrix<float>(Vec3f(alignedPos, element.GetDepth())));
			program->SetUniform(1, element.GetScene()->GetProjectionMatrix());
			program->SetUniform(2, 0);
			program->SetUniform(3, (Vec4f)text.color);
			program->SetUniform(4, Vec4f(clipRect.pos, clipRect.size));

			Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Points, 0, text.vertices.size());

		}
	}
}