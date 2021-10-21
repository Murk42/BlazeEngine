#include "BlazeEngine/Graphics/UI/Elements/TextRenderer.h"
#include "BlazeEngine/Graphics/UI/Elements/Text.h"
#include "BlazeEngine/Graphics/UI/Elements/ElementManager.h"
#include "BlazeEngine/Resources/Font/Font.h"

namespace Blaze::UI
{
	TextRenderer::TextRenderer()
		: uniformVP(-1), uniformTex(-1)
	{
	}

	void TextRenderer::Load()
	{
		Graphics::Core::VertexShader vertexShader;
		Graphics::Core::GeometryShader geometryShader;
		Graphics::Core::FragmentShader fragmentShader;

		vertexShader.Load("assets/ui/text/shader.vert");
		geometryShader.Load("assets/ui/text/shader.geom");
		fragmentShader.Load("assets/ui/text/shader.frag");

		program.LinkShaders({ &vertexShader, &fragmentShader, &geometryShader });
		uniformVP = program.GetUniformLocation("u_MVP");
		uniformTex = program.GetUniformLocation("u_texture");


		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.EnableVertexAttribute(4);
		va.SetVertexAttributeData(0, Graphics::Core::VertexAttribute(&vb, sizeof(TextVertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, 0));
		va.SetVertexAttributeData(1, Graphics::Core::VertexAttribute(&vb, sizeof(TextVertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 2));
		va.SetVertexAttributeData(2, Graphics::Core::VertexAttribute(&vb, sizeof(TextVertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 4));
		va.SetVertexAttributeData(3, Graphics::Core::VertexAttribute(&vb, sizeof(TextVertex), 0, Graphics::Core::VertexAttributeType::Float, 2, false, sizeof(float) * 6));
		va.SetVertexAttributeData(4, Graphics::Core::VertexAttribute(&vb, sizeof(TextVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, false, sizeof(float) * 8));
	}

	void TextRenderer::AddText(Text& text)
	{
		text.renderer = this;		
		texts.emplace_back(&text);
	}

	void TextRenderer::Render(const Mat4f& VP)
	{
		for (auto& text : this->texts)
		{
			vb.AllocateDynamicStorage(BufferView(text->vertices, text->vertexCount * sizeof(TextVertex)), Graphics::Core::GraphicsBufferDynamicStorageHint::StaticDraw);

			Renderer::SelectProgram(&program);
			program.SetUniform<int>(uniformTex, 0);
			program.SetUniform<Mat4f>(uniformVP, VP * text->transform.mat);

			Renderer::SelectVertexArray(&va);

			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(&text->GetFont()->GetTexture());

			if (text->clip)
			{
				Renderer::EnableScissorTest(true);
				Renderer::SetScissorRect(text->clipRect.pos, text->clipRect.size);
			}

			Renderer::RenderPointArray(text->vertexCount, 0);

			if (text->clip)
				Renderer::EnableScissorTest(false);
		}
	}
}