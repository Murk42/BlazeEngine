#pragma once
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Core/UINode.h"

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
		public Graphics::RenderUnit,
		private EventHandler<Node::TransformUpdatedEvent>,
		private EventHandler<Node::FinalTransformUpdatedEvent>
	{
	public:
		ImageRenderUnit(Node* node);

		void BeginStream() override;
		void* Advance() override;

		void SetTexture(Graphics::OpenGLWrapper::Texture2D* texture);
		void SetSourceRect(Vec2f uv1, Vec2f uv2);
		void SetBlendColor(ColorRGBAf color);
		void SetBlendFactor(float blend);
		void SetAlphaMultiplier(float alpha);
		void SetImageLayout(ImageLayout layout);

		inline Graphics::OpenGLWrapper::Texture2D* GetTexture() const { return rd.texture; }
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

		void OnEvent(Node::TransformUpdatedEvent event) override;
		void OnEvent(Node::FinalTransformUpdatedEvent event) override;

		//Cleans renderData if it is dirty, if not does nothing
		void CleanRenderData();
	};
}