#pragma once
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/ImageRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze::UI::Nodes
{	
	class Image : 
		public Node,
		public Graphics::RenderObject
	{
	public:
		Image();

		inline void SetTexture(ResourceBaseRef texture) { renderUnit.SetTexture(texture); }
		inline void SetSourceRect(Vec2f uv1, Vec2f uv2) { renderUnit.SetSourceRect(uv1, uv2); }
		inline void SetBlendColor(ColorRGBAf color) { renderUnit.SetBlendColor(color); }		
		inline void SetBlendFactor(float blend) { renderUnit.SetBlendFactor(blend); }
		inline void SetAlphaMultiplier(float alpha) { renderUnit.SetAlphaMultiplier(alpha); }
		inline void SetImageLayout(ImageLayout layout) { renderUnit.SetImageLayout(layout); }

		inline const ResourceBaseRef GetTexture() const { return renderUnit.GetTexture(); }
		inline Rectf GetSourceRect() const { return renderUnit.GetSourceRect(); }
		inline ColorRGBAf GetBlendColor() const { return renderUnit.GetBlendColor(); }
		inline float GetBlendFactor() const { return renderUnit.GetBlendFactor(); }
		inline float GetAlphaMultiplier() const { return renderUnit.GetAlphaMultiplier(); }
		inline ImageLayout GetImageLayout() const { return renderUnit.GetImageLayout(); }
		
		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:				
		ImageRenderUnit renderUnit;		
	};
}