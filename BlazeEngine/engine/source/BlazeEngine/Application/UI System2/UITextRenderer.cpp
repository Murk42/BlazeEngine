#include "BlazeEngine/Application/UI System2/UITextRenderer.h"
#include "BlazeEngine/Graphics/Renderers/TextVertexGenerator.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze::UI2
{
	/*
	struct Vertex
	{
		Vec2f p1;
		Vec2f p2;
		Vec2f uv1;
		Vec2f uv2;
	};

	Graphics::Core::ShaderProgram* UITextRenderer::SelectProgram(FontResolutionRenderType renderType)
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

	void UITextRenderer::RenderAll()
	{
		vb.FlushBufferRange(0, usedVBSize);
		fence.SetFence();

		Vec2i viewportSize = Graphics::Core::GetViewportSize();
		Mat4f vp2d = Math::OrthographicMatrix(0, viewportSize.x, 0, viewportSize.y, -1, 1);

		if (fontResolution == nullptr)
		{
			Logger::AddLog(Log(LogType::Warning, BLAZE_FILE_NAME, BLAZE_FUNCTION_NAME, BLAZE_FILE_LINE, "BlazeEngine",
				"Text has its font resolution set to nullptr"));
			return;
		}

		auto* program = SelectProgram(fontResolution->GetRenderType());

		Graphics::Core::SelectProgram(program);

		Graphics::Core::SelectVertexArray(&va);
		Graphics::Core::SetActiveTextureSlot(0);
		Graphics::Core::SelectTexture(&fontResolution->GetAtlas());
		program->SetUniform(0, vp2d);
		program->SetUniform(1, 0);

		fence.BlockServer();
		Graphics::Core::RenderIndexedPrimitives(Graphics::Core::PrimitiveType::Triangles, Graphics::Core::IndexType::Uint32, GetUsedIBSize() / sizeof(uint32), 0);
	}

	UITextRenderer::UITextRenderer()
	{
		Graphics::Core::VertexShader vert{ "assets/default/shaders/ui_text.vert" };
		Graphics::Core::GeometryShader geom{ "assets/default/shaders/ui_text.geom" };
		Graphics::Core::FragmentShader fragNormal{ "assets/default/shaders/ui_text_normal.frag" };
		Graphics::Core::FragmentShader fragSDF{ "assets/default/shaders/ui_text_sdf.frag" };
		Graphics::Core::FragmentShader fragLCD{ "assets/default/shaders/ui_text_lcd.frag" };

		programNormal.LinkShaders({ &vert, &geom, &fragNormal });
		programSDF.LinkShaders({ &vert, &geom, &fragSDF });
		programLCD.LinkShaders({ &vert, &geom, &fragLCD });

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
	void UITextRenderer::ReserveVB(uint size)
	{
		reservedVBSize = size;
		usedVBSize = 0;

		vb.AllocateStaticStorage({ nullptr, size },
			Graphics::Core::GraphicsBufferStaticStorageHint::MapPersistant |
			Graphics::Core::GraphicsBufferStaticStorageHint::MapWrite);
		vbMap = vb.MapBufferRange(0, size,
			Graphics::Core::GraphicsBufferMapAccess::Persistant |
			Graphics::Core::GraphicsBufferMapAccess::Write);
	}


	Rectf UITextRenderer::RenderText(TextRenderData data)
	{
		if (fontResolution != data.fontResolution)
			RenderAll();

		Graphics::DefaultTextVertexGenerator generator;
		generator.Setup(data.text, fontResolution);

		Vec2f bottomLeft = { };
		Vec2f topRight = { };

		float scale = (float)data.fontHeight / fontResolution->GetResolution();

		while (!generator.IsEnd())
		{
			if (usedVBSize + sizeof(Vertex) >= reservedVBSize)
				RenderAll();

			Vertex& vertex = *(Vertex*)((uint8*)vbMap + usedVBSize);

			float next;
			if (generator.GenerateVertex(vertex.p1, vertex.p2, vertex.uv1, vertex.uv2, next))
			{
				vertex.p1 *= scale;
				vertex.p2 *= scale;

				bottomLeft.x = std::min({ bottomLeft.x, vertex.p1.x, vertex.p2.x });
				bottomLeft.y = std::min({ bottomLeft.y, vertex.p1.y, vertex.p2.y });
				topRight.x = std::max({ topRight.x, vertex.p1.x, vertex.p2.x });
				topRight.y = std::max({ topRight.y, vertex.p1.y, vertex.p2.y });
			}
		}

	}
	*/	

}