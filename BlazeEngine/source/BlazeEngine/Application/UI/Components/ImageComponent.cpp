#include "pch.h"
#include "BlazeEngine/Application/UI/Components/ImageComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"

namespace Blaze::UI::Components 
{
	void ImageSystem::Render(UIScene* scene, UIComponent* component)
	{
		auto& image = *(Image*)component;

		//auto& imageRenderer = Graphics::Core::GetDefaultTexture2DRenderer();
		//
		//Graphics::ImageGraphicsData data {.size = image.transform->GetFinalSize(), .texture = image.texture, .uvs = {
		//	Vec2f(image.uv1.x, image.uv1.y),
		//	Vec2f(image.uv1.x, image.uv2.y),
		//	Vec2f(image.uv2.x, image.uv1.y),
		//	Vec2f(image.uv2.x, image.uv2.y),
		//}, .mask = image.mask, .blendFactor = image.blend };
		//
		//imageRenderer.Render(image.transform->GetMatrix(), data);
	}

	Image::Image(const Image& other)
		: uv1(other.uv1), uv2(other.uv2), mask(other.mask), blend(other.blend)
	{
	}
	Image::Image(Vec2f uv1, Vec2f uv2, ColorRGBAf mask, float blend)
		: uv1(uv1), uv2(uv2), mask(mask), blend(blend)
	{ 
	}	
	void Image::SetUVs(Vec2f uv1, Vec2f uv2)
	{
		this->uv1 = uv1;
		this->uv2 = uv2;
	}
	void Image::SetMask(ColorRGBAf mask, float blend)
	{
		this->mask = mask;
		this->blend = blend;
	}	
}