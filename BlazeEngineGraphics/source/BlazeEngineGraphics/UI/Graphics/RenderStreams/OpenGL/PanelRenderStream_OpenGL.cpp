#include "pch.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/PanelRenderStream_OpenGL.h"
#include "BlazeEngineGraphics/UI/Elements/UIPanel.h"

namespace Blaze::Graphics::OpenGL
{
	PanelRenderStream_OpenGL::PanelRenderStream_OpenGL(UI::Panel* panel)
		: panel(panel)
	{

	}
	void PanelRenderStream_OpenGL::BeginStream()
	{
		advanced = false;
	}
	void* PanelRenderStream_OpenGL::GetCurrent()
	{
		if (advanced)
			return &rd;

		Vec2f pos = panel->GetFinalPosition();
		Vec2f size = panel->GetFinalSize();
		float rotation = panel->GetFinalRotation();		

		rd = {
			.pos = pos,
			.size = size,
			.rotation = rotation,
			.borderWidth = panel->borderWidth * panel->GetFinalScale(),
			.cornerRadius = panel->cornerRadius * panel->GetFinalScale(),
			.fillColor = panel->fillColor,
			.borderColor = panel->borderColor,			
		};

		return &rd;
	}
	void PanelRenderStream_OpenGL::Advance()
	{
		advanced = true;
	}
	bool PanelRenderStream_OpenGL::IsEmpty()
	{
		return advanced;
	}
}