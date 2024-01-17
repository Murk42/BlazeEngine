#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/TextRenderer_OpenGL.h"

#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

#include "BlazeEngineGraphics/Core/StreamRenderer.h"

namespace Blaze::Graphics::OpenGL
{	
	class UIRenderPipeline_OpenGL :
		EventHandler<UI::NodeCreatedEvent>,
		EventHandler<UI::NodeDestroyedEvent>,
		EventHandler<UI::ScreenDestructionEvent>
	{
	public:
		UIRenderPipeline_OpenGL(
			TexturedRectRenderer_OpenGL& texturedRectRenderer,
			PanelRenderer_OpenGL& panelRenderer,
			TextRenderer_OpenGL& textRenderer);
		~UIRenderPipeline_OpenGL();

		void SetScreen(UI::Screen* newScreen);
		void Render(Vec2u targetSize);
	private:
		UI::Screen* screen;
		TexturedRectRenderer_OpenGL& texturedRectRenderer;
		PanelRenderer_OpenGL& panelRenderer;
		TextRenderer_OpenGL& textRenderer;	
		bool recreateRenderQueue;
				
		struct NodeRenderGroup
		{
			uint count;
			StreamRenderer* renderer;			
		};
		
		Array<UI::Node*> renderQueue;
		Array<NodeRenderGroup> renderGroups;

		StreamRenderer* GetRenderer(UI::Node* node);		
			
		void RecreateRenderQueue();

		void OnEvent(UI::NodeCreatedEvent event) override;
		void OnEvent(UI::NodeDestroyedEvent event) override;
		void OnEvent(UI::ScreenDestructionEvent event) override;
		
		friend class UIRenderPipelineRenderStream_OpenGL;
		friend class UIBestRenderQueue;
		friend class UISimpleRenderQueue;
	};
}