#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Graphics/GraphicsCore.h"
#include "BlazeEngine/Math/Math.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

namespace Blaze::UI
{
	struct ImageVertex
	{
		Vec3f pos;
		Vec2f uv;
		Vec4f color;
	};

	Image::Image()
		: texture(nullptr), mask(0x00000000)
	{		
	}	

	void Image::SetProperties(const ImageProperties& p)
	{
		if (p.texture.edited) texture = p.texture;
		if (p.sourceRect.edited) sourceRect = p.sourceRect;
		if (p.mask.edited) mask = p.mask;
	}

	void ImageManager::Setup()
	{
		imagesVA.EnableVertexAttribute(0);
		imagesVA.EnableVertexAttribute(1);
		imagesVA.EnableVertexAttribute(2);
		imagesVA.SetVertexAttributeFormat(0, Graphics::Core::VertexAttributeType::Float, 3, false, 0);
		imagesVA.SetVertexAttributeFormat(1, Graphics::Core::VertexAttributeType::Float, 2, false, 12);
		imagesVA.SetVertexAttributeFormat(2, Graphics::Core::VertexAttributeType::Float, 4, false, 20);
		imagesVA.SetVertexAttributeBuffer(0, &imagesVB, sizeof(ImageVertex), 0);
		imagesVA.SetVertexAttributeBuffer(1, &imagesVB, sizeof(ImageVertex), 0);
		imagesVA.SetVertexAttributeBuffer(2, &imagesVB, sizeof(ImageVertex), 0);

		Graphics::Core::VertexShader vs{ "assets/default/shaders/ui_image.vert" };
		Graphics::Core::FragmentShader fs{ "assets/default/shaders/ui_image.frag" };

		imagesSP.LinkShaders({ &vs, &fs });
	}
	void ImageManager::Render(UIElement* element)
	{		
		Image& image = *(Image*)element;

		if (!image.IsActive())
			return; 
		 
		Graphics::Core::SelectProgram(&imagesSP);

		Graphics::Core::SelectVertexArray(&imagesVA);

		Vec2f pos = image.GetViewportPos();
		Vec2f size = image.GetSize();
		float depth = image.GetDepth();
		Rectf clipRect = image.GetClipRect();

		ImageVertex vertices[6];
		vertices[0].pos = Vec3f(pos, depth);							vertices[0].uv = image.sourceRect.pos + Vec2f(0, image.sourceRect.h);	vertices[0].color = (Vec4f)image.mask;
		vertices[1].pos = Vec3f(pos + Vec2f(0, size.y), depth);	vertices[1].uv = image.sourceRect.pos;									vertices[1].color = (Vec4f)image.mask;
		vertices[2].pos = Vec3f(pos + Vec2f(size.x, 0), depth);	vertices[2].uv = image.sourceRect.pos + image.sourceRect.size;			vertices[2].color = (Vec4f)image.mask;
		vertices[3].pos = Vec3f(pos + Vec2f(size.x, 0), depth);	vertices[3].uv = image.sourceRect.pos + image.sourceRect.size;			vertices[3].color = (Vec4f)image.mask;
		vertices[4].pos = Vec3f(pos + Vec2f(0, size.y), depth);	vertices[4].uv = image.sourceRect.pos;									vertices[4].color = (Vec4f)image.mask;
		vertices[5].pos = Vec3f(pos + size, depth);			vertices[5].uv = image.sourceRect.pos + Vec2f(image.sourceRect.w, 0);	vertices[5].color = (Vec4f)image.mask;

		imagesVB.AllocateDynamicStorage({ vertices, sizeof(vertices) }, Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);

		imagesSP.SetUniform(0, GetManager()->GetProjectionMatrix());
		imagesSP.SetUniform(1, 0);
		imagesSP.SetUniform(2, Vec4f(clipRect.pos, clipRect.size));

		Graphics::Core::SetActiveTextureSlot(0);
		Graphics::Core::SelectTexture(image.texture);

		Graphics::Core::RenderPrimitiveArray(Graphics::Core::PrimitiveType::Triangles, 0, 6);
	}
}