#pragma once
#include "BlazeEngineGraphics/Renderers/PanelRenderer.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Core/UINode.h"

namespace Blaze::UIGraphics
{
	class PanelNodeRenderUnit :
		public Graphics::RenderUnit,
		private EventHandler<UI::Node::FinalTransformUpdatedEvent>
	{
	public:
		PanelNodeRenderUnit(UI::Node* node);

		void BeginStream() override;		
		void* Advance() override;			

		void SetFillColor(ColorRGBAf color);
		void SetBorderColor(ColorRGBAf color);
		void SetCornerRadius(float radius);
		void SetBorderWidth(float width);

		inline ColorRGBAf GetFillColor() const { return rd.fillColor; }
		inline ColorRGBAf GetBorderColor() const { return rd.borderColor; }
		inline float GetCornerRadius() const { return rd.cornerRadius; }
		inline float GetBorderWidth() const { return rd.borderWidth; }
	private:
		Graphics::PanelRenderData_OpenGL rd;

		UI::Node* node;
		bool rendered : 1;		

		void OnEvent(UI::Node::FinalTransformUpdatedEvent event);
	};
}