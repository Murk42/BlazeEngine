#pragma once
#include "BlazeEngineCore/DataStructures/Rect.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI
{
	enum class ImageLayout
	{
		Fit,
		Stretch,
		Fill,
		KeepWidth,
		KeepHeight
	};

	class ImageRenderUnit :
		public Graphics::RenderUnit		
	{
	public:
		ImageRenderUnit(Node* node);
		~ImageRenderUnit();

		void BeginStream() override;
		void* Advance() override;

		void SetTexture(Graphics::OpenGLWrapper::Texture2D* texture);
		void SetSourceRect(Vec2f uv1, Vec2f uv2);
		void SetBlendColor(ColorRGBAf color);
		void SetBlendFactor(float blend);
		void SetAlphaMultiplier(float alpha);
		void SetImageLayout(ImageLayout layout);

		inline const Graphics::OpenGLWrapper::Texture2D* GetTexture() const { return rd.texture; }
		inline Rectf GetSourceRect() const { return Rectf(uv1, uv2 - uv1); }
		inline ColorRGBAf GetBlendColor() const { return rd.color; }
		inline float GetBlendFactor() const { return rd.blend; }
		inline float GetAlphaMultiplier() const { return rd.alpha; }
		inline ImageLayout GetImageLayout() const { return layout; }
	private:
		Node* node;

		bool rendered : 1;
		bool renderDataDirty : 1;
		Vec2f uv1, uv2;
		ImageLayout layout;

		Graphics::TexturedRectRenderData rd;

		void TransformUpdatedEvent(const Node::TransformUpdatedEvent& event);
		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);

		//Cleans renderData if it is dirty, if not does nothing
		void CleanRenderData();
	};
}