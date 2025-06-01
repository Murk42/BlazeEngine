#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/ColoredCharacterRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"

#include "BlazeEngineGraphics/UI/Core/Screen.h"

#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::Graphics::OpenGL
{
	struct UINodeRenderGroup
	{
		uintMem count;
		StreamRenderer& renderer;
	};
	struct UIRenderItem
	{
		RenderUnit& renderUnit;
		UI::Node& node;
	};

	class UIRenderPipeline_OpenGL
	{
	public:
		UIRenderPipeline_OpenGL(
			TexturedRectRenderer_OpenGL& texturedRectRenderer,
			TexturedRectRenderer_OpenGL& coloredCharacterRenderer_OpenGL,
			PanelRenderer_OpenGL& panelRenderer
			);
		~UIRenderPipeline_OpenGL();

		void SetScreen(UI::Screen* newScreen);
		void Render(Vec2u targetSize);		

		StreamRenderer* GetRenderer(RenderUnit& renderUnit);
		
		inline UI::Screen* GetScreen() const { return screen; }
	private:
		UI::Screen* screen;
		TexturedRectRenderer_OpenGL& texturedRectRenderer;
		TexturedRectRenderer_OpenGL& coloredCharacterRenderer_OpenGL;
		PanelRenderer_OpenGL& panelRenderer;			
		bool recreateRenderQueue;				
		
		Array<UIRenderItem> renderQueue;
		Array<UINodeRenderGroup> renderGroups;

			
		void RecreateRenderQueue();
		
		void NodeTreeChanged(const UI::NodeTreeChangedEvent& event);
		void ScreenDestroyed(const UI::ScreenDestructionEvent& event);
		void NodeEnabledStateUpdated(const UI::Node::EnabledStateChangedEvent& event);
	};
}