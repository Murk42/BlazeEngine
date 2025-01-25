#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/ImageRenderUnit.h"

namespace Blaze::UI
{
	ImageRenderUnit::ImageRenderUnit(Node* node)
		: RenderUnit("TexturedRectRenderer"), node(node), rendered(true), renderDataDirty(false), layout(ImageLayout::Stretch), 
		uv1(0, 0), uv2(1, 1), 
		rd({
			.texture = nullptr,
			.uv1 = Vec2f(0, 0),
			.uv2 = Vec2f(1, 1),
			.pos = Vec2f(0, 0),
			.right = Vec2f(1, 0),
			.up = Vec2f(0, 1),
			.color = 0xffffffff,
			.blend = 0,
			.alpha = 1,
			})
	{
		node->finalTransformUpdatedEventDispatcher.AddHandler(*this);
		node->transformUpdatedEventDispatcher.AddHandler(*this);
	}
	void ImageRenderUnit::BeginStream()
	{				
		CleanRenderData();

		rendered = false;
	}	
	void* ImageRenderUnit::Advance()
	{
		if (rendered)
			return nullptr;		

		rendered = true;				

		return &rd;
	}
	void ImageRenderUnit::SetTexture(Graphics::OpenGLWrapper::Texture2D* texture)
	{
		if (texture == rd.texture)
			return;

		rd.texture = texture;

		switch (layout)
		{
		case ImageLayout::Fit:
			renderDataDirty = true;			
			break;
		case ImageLayout::Fill:
			renderDataDirty = true;
			break;
		case ImageLayout::KeepWidth:			
		case ImageLayout::KeepHeight:			
			node->MarkTransformDirty();			
			break;
		case ImageLayout::Stretch:
		default:			
			break;
		}
	}
	void ImageRenderUnit::SetSourceRect(Vec2f uv1, Vec2f uv2)
	{
		this->uv1 = uv1;
		this->uv2 = uv2;
		renderDataDirty = true;
	}
	void ImageRenderUnit::SetBlendColor(ColorRGBAf color)
	{
		rd.color = color;
	}
	void ImageRenderUnit::SetBlendFactor(float blend)
	{
		rd.blend = blend;
	}
	void ImageRenderUnit::SetAlphaMultiplier(float alpha)
	{
		rd.alpha = alpha;
	}
	void ImageRenderUnit::SetImageLayout(ImageLayout layout)
	{
		if (this->layout == layout)
			return;

		renderDataDirty = true;
		this->layout = layout;		
	}
	void ImageRenderUnit::OnEvent(Node::TransformUpdatedEvent event)
	{
		if (rd.texture == nullptr)
			return;

		NodeTransform transform = node->GetTransform();
		Vec2f textureSize = (uv2 - uv1) * (Vec2f)rd.texture->GetSize();

		switch (layout)
		{
		case ImageLayout::KeepHeight:
			transform.size.x = textureSize.x * transform.size.y / textureSize.y;
			break;
		case ImageLayout::KeepWidth:
			transform.size.y = textureSize.y * transform.size.x / textureSize.x;			
			break;		
		default:
			break;
		}

		node->SetTransform(transform);
	}
	void ImageRenderUnit::OnEvent(Node::FinalTransformUpdatedEvent event)
	{				
		renderDataDirty = true;
	}
	void ImageRenderUnit::CleanRenderData()
	{
		node->CleanFinalTransform();

		if (!renderDataDirty)
			return;

		renderDataDirty = false;

		auto finalTransform = node->GetFinalTransform();
		Vec2f& pos = finalTransform.position;
		Vec2f& size = finalTransform.size;
		float& rotation = finalTransform.rotation;

		const float cos = Math::Cos(rotation);
		const float sin = Math::Sin(rotation);
		const Vec2f right = Vec2f(cos, sin);
		const Vec2f up = Vec2f(-sin, cos);

		Rectf sourceRect = GetSourceRect();

		switch (layout)
		{
		case ImageLayout::Fit: {

			if (rd.texture == nullptr)
				break;

			Vec2f textureSize = (Vec2f)rd.texture->GetSize();
			sourceRect.size *= textureSize;
			sourceRect.pos *= textureSize;
			float frameRatio = size.y / size.x;
			float textureRatio = sourceRect.h / sourceRect.w;

			if (frameRatio > textureRatio)
			{
				float newHeight = textureRatio * size.x;

				pos += up * (size.y - newHeight) / 2;
				size.y = newHeight;
			}
			else
			{
				float newWidth = size.y / textureRatio;

				pos += right * (size.x - newWidth) / 2;
				size.x = newWidth;
			}

			sourceRect.size /= textureSize;
			sourceRect.pos /= textureSize;

			break;
		}						
		case ImageLayout::Fill: {

			if (rd.texture == nullptr)
				break;

			Vec2f textureSize = (Vec2f)rd.texture->GetSize();

			sourceRect.size *= textureSize;
			sourceRect.pos *= textureSize;

			float frameRatio = size.y / size.x;
			float textureRatio = sourceRect.h / sourceRect.w;

			if (frameRatio < textureRatio)
			{
				float newHeight = frameRatio * sourceRect.w;

				sourceRect.y += (sourceRect.h - newHeight) / 2;
				sourceRect.h = newHeight;
			}
			else
			{
				float newWidth = sourceRect.h / frameRatio;

				sourceRect.x += (sourceRect.w - newWidth) / 2;
				sourceRect.w = newWidth;
			}

			sourceRect.size /= textureSize;
			sourceRect.pos /= textureSize;

			break;
		}			
		case ImageLayout::Stretch:
		case ImageLayout::KeepWidth:			
		case ImageLayout::KeepHeight:			
		default:
			break;
		}		

		rd.uv1 = sourceRect.pos;
		rd.uv2 = sourceRect.pos + sourceRect.size;
		rd.pos = pos;
		rd.right = right * size.x;
		rd.up = up * size.y;
	}
}