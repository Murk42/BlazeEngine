#include "BlazeEngine/Graphics/UI/Elements/Panel.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze::UI
{
	PanelRenderer::PanelRenderer()
		: uniformVP(-1)
	{
	}

	PanelRenderer::~PanelRenderer()
	{
	}

	void PanelRenderer::AddPanel(Panel& panel)
	{
		panels.emplace_back(&panel);
		panel.renderer = this;
	}

	struct PanelVertex
	{
		Mat4f mat;
		Vec2f size;
		Vec4f color;
		Vec4f outlineColor;
		float radius;
		float thickness;
	};

	void PanelRenderer::Load()
	{
		Graphics::Core::VertexShader vertexShader;
		Graphics::Core::GeometryShader geometryShader;
		Graphics::Core::FragmentShader fragmentShader;

		vertexShader.Load("assets/ui/panel/shader.vert");
		geometryShader.Load("assets/ui/panel/shader.geom");
		fragmentShader.Load("assets/ui/panel/shader.frag");

		program.LinkShaders({ &vertexShader, &fragmentShader, &geometryShader });
		uniformVP = program.GetUniformLocation("u_PV");

		va.EnableVertexAttribute(0);
		va.EnableVertexAttribute(1);
		va.EnableVertexAttribute(2);
		va.EnableVertexAttribute(3);
		va.EnableVertexAttribute(4);
		va.EnableVertexAttribute(5);
		va.EnableVertexAttribute(6);
		va.EnableVertexAttribute(7);
		va.EnableVertexAttribute(8);
		va.SetVertexAttributeData(0, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, false, 0));
		va.SetVertexAttributeData(1, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, false, sizeof(float) * 4));
		va.SetVertexAttributeData(2, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, false, sizeof(float) * 8));
		va.SetVertexAttributeData(3, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, false, sizeof(float) * 12));
		va.SetVertexAttributeData(4, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 2, true, sizeof(float) * 16));
		va.SetVertexAttributeData(5, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, true, sizeof(float) * 18));
		va.SetVertexAttributeData(6, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 4, true, sizeof(float) * 22));
		va.SetVertexAttributeData(7, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 1, true, sizeof(float) * 26));
		va.SetVertexAttributeData(8, Graphics::Core::VertexAttribute(&vb, sizeof(PanelVertex), 0, Graphics::Core::VertexAttributeType::Float, 1, true, sizeof(float) * 27));
	}

	constexpr size_t vertexCount = 128;
	PanelVertex vertices[vertexCount];

	void PanelRenderer::Render(Mat4f& vp)
	{
		uint i = 0;
		while (i < panels.size())
		{
			uint j = 0;
			while (j < vertexCount && i < panels.size())
			{
				vertices[j].mat = panels[i]->transform.mat;
				vertices[j].color = panels[i]->color;
				vertices[j].size = panels[i]->transform.absoluteSize;
				vertices[j].outlineColor = panels[i]->outlineColor;
				vertices[j].radius = panels[i]->radius;
				vertices[j].thickness = panels[i]->thickness;
				++i;
				++j;
			}

			vb.AllocateDynamicStorage(BufferView(vertices, j * sizeof(PanelVertex)), Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);

			Renderer::SelectVertexArray(&va);
			Renderer::SelectProgram(&program);
			program.SetUniform(uniformVP, vp);

			Renderer::RenderPointArray(j, 0);
		}
	}
}