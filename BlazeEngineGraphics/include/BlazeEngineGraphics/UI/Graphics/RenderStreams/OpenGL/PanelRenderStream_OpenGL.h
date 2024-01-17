#pragma once
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Core/RenderStream.h"

namespace Blaze::UI
{
	class Panel;
}

namespace Blaze::Graphics::OpenGL
{
	class PanelRenderStream_OpenGL :
		public RenderStream
	{
		UI::Panel* panel;
		bool advanced;
		PanelRenderData_OpenGL rd;
	public:
		PanelRenderStream_OpenGL(UI::Panel* panel);

		void BeginStream() override;
		void* GetCurrent() override;
		void Advance() override;
		bool IsEmpty() override;
	};
}