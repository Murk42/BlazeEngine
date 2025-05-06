#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/ColoredCharacterRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"

#include "BlazeEngineGraphics/UI/Core/Screen.h"

#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::Graphics::OpenGL
{			
	class UIRenderPipeline_OpenGL :
		EventHandler<UI::NodeTreeChangedEvent>,		
		EventHandler<UI::ScreenDestructionEvent>
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
	private:
		UI::Screen* screen;
		TexturedRectRenderer_OpenGL& texturedRectRenderer;
		TexturedRectRenderer_OpenGL& coloredCharacterRenderer_OpenGL;
		PanelRenderer_OpenGL& panelRenderer;			
		bool recreateRenderQueue;
				
		struct NodeRenderGroup
		{
			uintMem count;
			StreamRenderer& renderer;			
		};
		struct RenderItem
		{
			RenderUnit& renderUnit;
			UI::Node& node;
		};
		
		Array<RenderItem> renderQueue;
		Array<NodeRenderGroup> renderGroups;

		StreamRenderer* GetRenderer(RenderUnit& renderUnit);
			
		void RecreateRenderQueue();
		
		void OnEvent(const UI::NodeTreeChangedEvent& event) override;
		void OnEvent(const UI::ScreenDestructionEvent& event) override;
		
		friend class GlobalRenderGroupStream;
		friend class UIBestRenderQueue;
		friend class UISimpleRenderQueue;
	};
}