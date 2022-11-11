#include "BlazeEngine/Application/UI System/Core Elements/TexturedPanel.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"

namespace Blaze
{ 
	namespace UI
	{
		struct Vertex
		{
			Vec3f pos;
			Vec2f uv;
			Vec4f color;
		};

		static void FillVertices(Vertex* vertices, Vec2f p1, Vec2f p2, float depth, Vec2f uv1, Vec2f uv2, ColorRGBAf mask)
		{
			vertices[0].pos = Vec3f(p1.x, p1.y, depth);	vertices[0].uv = Vec2f(uv1.x, uv1.y); vertices[0].color = (Vec4f)mask;
			vertices[1].pos = Vec3f(p1.x, p2.y, depth);	vertices[1].uv = Vec2f(uv1.x, uv2.y); vertices[1].color = (Vec4f)mask;
			vertices[2].pos = Vec3f(p2.x, p1.y, depth);	vertices[2].uv = Vec2f(uv2.x, uv1.y); vertices[2].color = (Vec4f)mask;
			vertices[3].pos = Vec3f(p2.x, p1.y, depth);	vertices[3].uv = Vec2f(uv2.x, uv1.y); vertices[3].color = (Vec4f)mask;
			vertices[4].pos = Vec3f(p1.x, p2.y, depth);	vertices[4].uv = Vec2f(uv1.x, uv2.y); vertices[4].color = (Vec4f)mask;
			vertices[5].pos = Vec3f(p2.x, p2.y, depth);	vertices[5].uv = Vec2f(uv2.x, uv2.y); vertices[5].color = (Vec4f)mask;
		}

		TexturedPanel::TexturedPanel()
			: mask(0, 0, 0, 0),
			sourceRect(0, 0, 100, 100), texture(nullptr)
		{
		}

		void TexturedPanel::SetProperties(const TexturedPanelProperties& p)
		{
			if (p.texture.edited) texture = p.texture.value;
			if (p.mask.edited) mask = p.mask.value;
			if (p.sourceRect.edited) sourceRect = p.sourceRect.value;
			if (p.textureSpacing.edited) textureSpacing = p.textureSpacing.value;
			if (p.spacing.edited) spacing = p.spacing.value;
		}

		TexturedPanelManager::TexturedPanelManager()
		{
			VA.EnableVertexAttribute(0);
			VA.EnableVertexAttribute(1);
			VA.EnableVertexAttribute(2);
			VA.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 3, false, 0);
			VA.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 2, false, 12);
			VA.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 4, false, 20);
			VA.SetVertexAttributeBuffer(0, &VB, sizeof(Vertex), 0);
			VA.SetVertexAttributeBuffer(1, &VB, sizeof(Vertex), 0);
			VA.SetVertexAttributeBuffer(2, &VB, sizeof(Vertex), 0);

			Graphics::Core::VertexShader vs{ "assets/default/shaders/ui_image.vert" };
			Graphics::Core::FragmentShader fs{ "assets/default/shaders/ui_image.frag" };

			SP.LinkShaders({ &vs, &fs });
		}
		void TexturedPanelManager::Setup()
		{
		}
		void TexturedPanelManager::Render(UIElement* element)
		{
			TexturedPanel& panel = *(TexturedPanel*)element;

			if (!panel.IsActive())
				return;

			Graphics::Core::SelectProgram(&SP);

			Graphics::Core::SelectVertexArray(&VA);

			Vec2f pos = panel.GetViewportPos();
			Vec2f size = panel.GetSize();
			float depth = panel.GetDepth();
			Rectf clipRect = panel.GetClipRect();

			Vec2f p1 = pos;
			Vec2f p4 = p1 + size;
			Vec2f p2 = p1 + Vec2f(panel.spacing.left, panel.spacing.top);
			Vec2f p3 = p4 - Vec2f(panel.spacing.right, panel.spacing.bottom);

			Vec2f uv1 = panel.sourceRect.pos;
			Vec2f uv4 = uv1 + panel.sourceRect.size;
			Vec2f uv2 = uv1 + Vec2f(panel.textureSpacing.left, panel.textureSpacing.top);
			Vec2f uv3 = uv4 - Vec2f(panel.textureSpacing.right, panel.textureSpacing.bottom);

			Vertex vertices[6 * 9];
			FillVertices(vertices +  0, Vec2f(p1.x, p1.y), Vec2f(p2.x, p2.y), depth, Vec2f(uv1.x, uv1.y), Vec2f(uv2.x, uv2.y), panel.mask);
			FillVertices(vertices +  6, Vec2f(p1.x, p2.y), Vec2f(p2.x, p3.y), depth, Vec2f(uv1.x, uv2.y), Vec2f(uv2.x, uv3.y), panel.mask);
			FillVertices(vertices + 12, Vec2f(p1.x, p3.y), Vec2f(p2.x, p4.y), depth, Vec2f(uv1.x, uv3.y), Vec2f(uv2.x, uv4.y), panel.mask);
			FillVertices(vertices + 18, Vec2f(p2.x, p1.y), Vec2f(p3.x, p2.y), depth, Vec2f(uv2.x, uv1.y), Vec2f(uv3.x, uv2.y), panel.mask);
			FillVertices(vertices + 24, Vec2f(p2.x, p2.y), Vec2f(p3.x, p3.y), depth, Vec2f(uv2.x, uv2.y), Vec2f(uv3.x, uv3.y), panel.mask);
			FillVertices(vertices + 30, Vec2f(p2.x, p3.y), Vec2f(p3.x, p4.y), depth, Vec2f(uv2.x, uv3.y), Vec2f(uv3.x, uv4.y), panel.mask);
			FillVertices(vertices + 36, Vec2f(p3.x, p1.y), Vec2f(p4.x, p2.y), depth, Vec2f(uv3.x, uv1.y), Vec2f(uv4.x, uv2.y), panel.mask);
			FillVertices(vertices + 42, Vec2f(p3.x, p2.y), Vec2f(p4.x, p3.y), depth, Vec2f(uv3.x, uv2.y), Vec2f(uv4.x, uv3.y), panel.mask);
			FillVertices(vertices + 48, Vec2f(p3.x, p3.y), Vec2f(p4.x, p4.y), depth, Vec2f(uv3.x, uv3.y), Vec2f(uv4.x, uv4.y), panel.mask);

			VB.AllocateDynamicStorage({ vertices, sizeof(vertices) }, Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);

			SP.SetUniform(0, GetManager()->GetProjectionMatrix());
			SP.SetUniform(1, 0);
			SP.SetUniform(2, Vec4f(clipRect.pos, clipRect.size));

			Graphics::Core::SetActiveTextureSlot(0);
			Graphics::Core::SelectTexture(panel.texture);

			Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Triangles, 0, 54);
		}

	}
}