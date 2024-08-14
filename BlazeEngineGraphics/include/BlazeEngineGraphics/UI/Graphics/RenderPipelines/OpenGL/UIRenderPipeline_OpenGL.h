#include "BlazeEngineGraphics/Renderers/OpenGL/TexturedRectRenderer_OpenGL.h"
#include "BlazeEngineGraphics/Renderers/OpenGL/PanelRenderer_OpenGL.h"

#include "BlazeEngineGraphics/UI/Core/UIScreen.h"

#include "BlazeEngineGraphics/RenderScene/RenderUnit.h"

namespace Blaze::Graphics::OpenGL
{		
	struct UIRenderPipelineDebugData
	{
		struct RenderGroup
		{
			Array<RenderStream*> renderUnits;
			StreamRenderer* renderer;
		};

		Array<RenderGroup> renderGroups;
	};

	class UIRenderPipeline_OpenGL :
		EventHandler<UI::NodeCreatedEvent>,
		EventHandler<UI::NodeDestroyedEvent>,
		EventHandler<UI::ScreenDestructionEvent>
	{
	public:
		UIRenderPipeline_OpenGL(
			TexturedRectRenderer_OpenGL& texturedRectRenderer,
			PanelRenderer_OpenGL& panelRenderer
			);
		~UIRenderPipeline_OpenGL();

		void SetScreen(UI::Screen* newScreen);
		void Render(Vec2u targetSize);

		void GetDebugData(UIRenderPipelineDebugData& debugData);
	private:
		UI::Screen* screen;
		TexturedRectRenderer_OpenGL& texturedRectRenderer;
		PanelRenderer_OpenGL& panelRenderer;			
		bool recreateRenderQueue;
				
		struct NodeRenderGroup
		{
			uintMem count;
			StreamRenderer* renderer;			
		};
		
		Array<RenderUnit*> renderQueue;
		Array<NodeRenderGroup> renderGroups;

		StreamRenderer* GetRenderer(RenderUnit* renderUnit);
			
		void RecreateRenderQueue();

		void OnEvent(UI::NodeCreatedEvent event) override;
		void OnEvent(UI::NodeDestroyedEvent event) override;
		void OnEvent(UI::ScreenDestructionEvent event) override;
		
		friend class UIRenderPipelineRenderStream_OpenGL;
		friend class UIBestRenderQueue;
		friend class UISimpleRenderQueue;
	};
}