#include "pch.h"
#include "BlazeEngine/UI/Nodes/Image.h"

#include "BlazeEngine/Graphics/Core/OpenGL/OpenGLWrapper/Textures/OpenGLTexture2D.h"

namespace Blaze::UI::Nodes
{
	Image::Image()
		: renderUnitDirty(true), imageLayout(ImageLayout::Fit), uv1(0), uv2(1)
	{
		dataMap.SetTypeName("Image");

		finalTransformUpdatedEventDispatcher.AddHandler<&Image::OnEvent>(*this);
	}
	Image::Image(Node& parent, const NodeTransform& transform, ResourceBaseRef texture, Vec2f uv1, Vec2f uv2, const Style& style)
		: Image()
	{
		SetParent(&parent);
		SetTransform(transform);
		SetTexture(texture, uv1, uv2);
		SetStyle(style);
	}
	Image::~Image()
	{
		finalTransformUpdatedEventDispatcher.RemoveHandler<&Image::OnEvent>(*this);
	}
	void Image::SetTexture(ResourceBaseRef texture, Vec2f uv1, Vec2f uv2)
	{
		renderUnit.texture = texture;
		this->uv1 = uv1;
		this->uv2 = uv2;
		renderUnitDirty = true;
	}
	void Image::SetStyle(const Style& style)
	{
		renderUnit.color = style.blendColor;
		renderUnit.blend = style.blendFactor;
		renderUnit.alpha = style.alphaMultiplier;
		imageLayout = style.layout;
		renderUnitDirty = true;
	}
	auto Image::GetStyle() const -> Style
	{
		Style style;
		style.blendColor = renderUnit.color;
		style.blendFactor = renderUnit.blend;
		style.alphaMultiplier = renderUnit.alpha;
		style.layout = imageLayout;
		return style;
	}
	void Image::PreRender(const UIRenderContext& renderContext)
	{
		CleanFinalTransform();

		if (!renderUnitDirty)
			return;

		renderUnitDirty = false;

		auto transform = GetFinalTransform();

		Vec2f& pos = transform.position;
		Vec2f& size = transform.size;
		float& rotation = transform.rotation;

		const float cos = Math::Cos(rotation);
		const float sin = Math::Sin(rotation);
		const Vec2f right = Vec2f(cos, sin);
		const Vec2f up = Vec2f(-sin, cos);

		Rectf finalSourceRect = { uv1, uv2 - uv1 };

		switch (imageLayout)
		{
		case ImageLayout::Fit: {

			if (!renderUnit.texture)
				break;

			Vec2f textureSize = (Vec2f)renderUnit.texture.Get<Graphics::OpenGL::Texture2D>().GetSize();
			finalSourceRect.size *= textureSize;
			finalSourceRect.pos *= textureSize;

			float frameRatio = size.y / size.x;
			float textureRatio = finalSourceRect.h / finalSourceRect.w;

			if (frameRatio < textureRatio)
			{
				float newWidth = size.y / textureRatio;

				pos += right * (size.x - newWidth) / 2;
				size.x = newWidth;
			}
			else
			{
				float newHeight = textureRatio * size.x;

				pos += up * (size.y - newHeight) / 2;
				size.y = newHeight;
			}

			finalSourceRect.size /= textureSize;
			finalSourceRect.pos /= textureSize;

			break;
		}
		case ImageLayout::Fill: {

			if (!renderUnit.texture)
				break;

			Vec2f textureSize = (Vec2f)renderUnit.texture.Get<Graphics::OpenGL::Texture2D>().GetSize();

			finalSourceRect.size *= textureSize;
			finalSourceRect.pos *= textureSize;

			float frameRatio = size.y / size.x;
			float textureRatio = finalSourceRect.h / finalSourceRect.w;

			if (frameRatio < textureRatio)
			{
				float newHeight = frameRatio * finalSourceRect.w;

				finalSourceRect.y += (finalSourceRect.h - newHeight) / 2;
				finalSourceRect.h = newHeight;
			}
			else
			{
				float newWidth = finalSourceRect.h / frameRatio;

				finalSourceRect.x += (finalSourceRect.w - newWidth) / 2;
				finalSourceRect.w = newWidth;
			}

			finalSourceRect.size /= textureSize;
			finalSourceRect.pos /= textureSize;

			break;
		}
		case ImageLayout::Stretch:
		case ImageLayout::KeepWidth:
		case ImageLayout::KeepHeight:
		default:
			break;
		}

		renderUnit.uv1 = finalSourceRect.pos;
		renderUnit.uv2 = finalSourceRect.pos + finalSourceRect.size;
		renderUnit.pos = pos;
		renderUnit.right = right * size.x;
		renderUnit.up = up * size.y;
	}
	UIRenderUnitBase* Image::GetRenderUnit(uintMem index)
	{
		return index == 0 ? &renderUnit : nullptr;
	}
	void Image::OnEvent(const Node::FinalTransformUpdatedEvent& event)
	{
		renderUnitDirty = true;
	}
}