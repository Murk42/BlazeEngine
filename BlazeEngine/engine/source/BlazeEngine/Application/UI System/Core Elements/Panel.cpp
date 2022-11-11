#include "BlazeEngine/Application/UI System/Core Elements/Panel.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{
	namespace UI
	{
		Panel::Panel()
			: borderWidth(1), cornerRadius(5), fillColor(0x000000ff), borderColor(0xffffffff)
		{			
		}

		Panel::~Panel()
		{

		}

		void Panel::SetProperties(const PanelProperties& p)
		{
			if (p.fillColor.edited) fillColor = p.fillColor;
			if (p.borderColor.edited) borderColor = p.borderColor;
			if (p.borderWidth.edited) borderWidth = p.borderWidth;
			if (p.cornerRadius.edited) cornerRadius = p.cornerRadius;
		}

		struct PanelVertex
		{
			Vec3f pos;
			Vec4f rect;
			Vec4f fillColor;
			Vec4f borderColor;
			float radius;
			float borderWidth;
		};

		void SetPoints(Vec2f& p, Vec2f& px, Vec2f& py, Vec2f& pxy, Rectf rect)
		{
			p = rect.pos;
			px = rect.pos + Vec2f(rect.w, 0);
			py = rect.pos + Vec2f(0, rect.h);
			pxy = rect.pos + rect.size;
		}
		void SetPoints(Vec3f& p, Vec3f& px, Vec3f& py, Vec3f& pxy, Rectf rect, float depth)
		{
			p = Vec3f(rect.pos, depth);
			px = Vec3f(rect.pos + Vec2f(rect.w, 0), depth);
			py = Vec3f(rect.pos + Vec2f(0, rect.h), depth);
			pxy = Vec3f(rect.pos + rect.size, depth);
		}

		void PanelManager::Setup()
		{
			panelsVA.EnableVertexAttribute(0);
			panelsVA.EnableVertexAttribute(1);
			panelsVA.EnableVertexAttribute(2);
			panelsVA.EnableVertexAttribute(3);
			panelsVA.EnableVertexAttribute(4);
			panelsVA.EnableVertexAttribute(5);
			panelsVA.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 3, false, 0);
			panelsVA.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 4, false, 12);
			panelsVA.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 4, false, 28);
			panelsVA.SetVertexAttributeFormat(3, Graphics::Core::VertexAttributeType::Float, 4, false, 44);
			panelsVA.SetVertexAttributeFormat(4, Graphics::Core::VertexAttributeType::Float, 1, false, 60);
			panelsVA.SetVertexAttributeFormat(5, Graphics::Core::VertexAttributeType::Float, 1, false, 64);
			panelsVA.SetVertexAttributeBuffer(0, &panelsVB, sizeof(PanelVertex), 0);
			panelsVA.SetVertexAttributeBuffer(1, &panelsVB, sizeof(PanelVertex), 0);
			panelsVA.SetVertexAttributeBuffer(2, &panelsVB, sizeof(PanelVertex), 0);
			panelsVA.SetVertexAttributeBuffer(3, &panelsVB, sizeof(PanelVertex), 0);
			panelsVA.SetVertexAttributeBuffer(4, &panelsVB, sizeof(PanelVertex), 0);
			panelsVA.SetVertexAttributeBuffer(5, &panelsVB, sizeof(PanelVertex), 0);

			Graphics::Core::VertexShader vs{ "assets/default/shaders/ui_panel.vert" };
			Graphics::Core::FragmentShader fs{ "assets/default/shaders/ui_panel.frag" };

			panelsSP.LinkShaders({ &vs, &fs });
		}		
		void PanelManager::Render(UIElement* element)
		{			
			Panel& panel = (Panel&)*element;

			PanelVertex vertices[6];

			if (!panel.IsActive())
				return;

			Vec2f pos = panel.GetViewportPos();
			Vec2f size = panel.GetSize();
			Rectf clipRect = panel.GetClipRect();

			Vec3f p1, p2, p3, p4;
			SetPoints(p1, p2, p3, p4, { pos, size }, panel.GetDepth());

			vertices[0].pos = p1;
			vertices[1].pos = p3;
			vertices[2].pos = p2;
			vertices[3].pos = p2;
			vertices[4].pos = p3;
			vertices[5].pos = p4;

			for (int i = 0; i < 6; ++i)
			{
				vertices[i].rect = Vec4f(pos, size);
				vertices[i].fillColor = (Vec4f)panel.fillColor;
				vertices[i].borderColor = (Vec4f)panel.borderColor;
				vertices[i].radius = panel.cornerRadius;
				vertices[i].borderWidth = panel.borderWidth;
			}

			Graphics::Core::SelectProgram(&panelsSP);
			Graphics::Core::SelectVertexArray(&panelsVA);
			panelsSP.SetUniform(0, GetManager()->GetProjectionMatrix());
			panelsSP.SetUniform(1, Vec4f(clipRect.pos, clipRect.size));

			panelsVB.AllocateDynamicStorage({ vertices, sizeof(PanelVertex) * 6 }, Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);


			Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Triangles, 0, 6);

		}
	
	}
}