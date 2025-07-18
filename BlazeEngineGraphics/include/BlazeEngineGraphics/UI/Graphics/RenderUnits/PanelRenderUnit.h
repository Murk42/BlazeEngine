#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/Renderers/PanelRenderer.h"
#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"
#include "BlazeEngineGraphics/UI/Core/Node.h"

namespace Blaze::UI
{	
	class BLAZE_GRAPHICS_API PanelRenderUnit :
		public Graphics::RenderUnit
	{
	public:
		PanelRenderUnit(Node* node);
		~PanelRenderUnit();

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

		Node* node;
		bool rendered : 1;		

		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}