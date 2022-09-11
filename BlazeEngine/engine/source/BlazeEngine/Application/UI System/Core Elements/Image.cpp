#include "BlazeEngine/Application/UI System/Core Elements/Image.h"
#include "BlazeEngine/Graphics/Renderer.h"
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
		: mask(0, 0, 0, 0), texture(nullptr)
	{
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
	void ImageManager::Render(size_t index, size_t end)
	{
		Vec2i size = Renderer::GetViewportSize();
		Mat4f vp2d = Math::OrthographicMatrix<float>(0, size.x, 0, size.y, -1000, 1000);

		for (; index != end; ++index)
		{
			Image& image = *GetElement(index);

			if (!image.IsActive())
				continue;

			Renderer::SelectProgram(&imagesSP);

			Renderer::SelectVertexArray(&imagesVA);

			Rectf alignedRect = image.GetAlignedRect();
			float depth = image.GetDepth();

			ImageVertex vertices[6];
			vertices[0].pos = Vec3f(alignedRect.pos, depth);							vertices[0].uv = image.sourceRect.pos + Vec2f(0, image.sourceRect.h);	vertices[0].color = (Vec4f)image.mask;
			vertices[1].pos = Vec3f(alignedRect.pos + Vec2f(0, alignedRect.h), depth);	vertices[1].uv = image.sourceRect.pos;									vertices[1].color = (Vec4f)image.mask;
			vertices[2].pos = Vec3f(alignedRect.pos + Vec2f(alignedRect.w, 0), depth);	vertices[2].uv = image.sourceRect.pos + image.sourceRect.size;			vertices[2].color = (Vec4f)image.mask;
			vertices[3].pos = Vec3f(alignedRect.pos + Vec2f(alignedRect.w, 0), depth);	vertices[3].uv = image.sourceRect.pos + image.sourceRect.size;			vertices[3].color = (Vec4f)image.mask;
			vertices[4].pos = Vec3f(alignedRect.pos + Vec2f(0, alignedRect.h), depth);	vertices[4].uv = image.sourceRect.pos;									vertices[4].color = (Vec4f)image.mask;
			vertices[5].pos = Vec3f(alignedRect.pos + alignedRect.size, depth);			vertices[5].uv = image.sourceRect.pos + Vec2f(image.sourceRect.w, 0);	vertices[5].color = (Vec4f)image.mask;

			imagesVB.AllocateDynamicStorage({ vertices, sizeof(vertices) }, Graphics::Core::GraphicsBufferDynamicStorageHint::DynamicDraw);

			imagesSP.SetUniform(0, vp2d);
			imagesSP.SetUniform(1, 0);
			Renderer::SetActiveTextureSlot(0);
			Renderer::SelectTexture(image.texture);

			Renderer::RenderPrimitiveArray(Renderer::PrimitiveType::Triangles, 0, 6);

		}
	}
	UIElementParsingData ImageManager::GetElementParsingData()
	{
		UIElementParsingData data = UIBaseElementManager::GetElementParsingData();
		data.AddMember("mask", &Image::mask);
		data.AddMember("sourceRect", &Image::sourceRect);
		data.AddMember<Image, String>("texture", 
			[](UIScene& scene, Image& image, String name) {
				image.texture = scene.GetResourceManager()->GetResource<Graphics::Core::Texture2D>(name);
			}, nullptr);
		return data;
	}
}