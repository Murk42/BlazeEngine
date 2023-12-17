#include "Graphics/OpenGL/Renderers/TexturedRectRenderer_OpenGL.h"
#include "Graphics/OpenGL/Renderers/PanelRenderer_OpenGL.h"
#include "Graphics/OpenGL/Renderers/TextRenderer_OpenGL.h"

#include "UI/UINode.h"

namespace Blaze::Graphics::OpenGL
{	
	class UIRenderPipeline_OpenGL : 
		private EventHandler<UI2::NodeCreatedEvent>,
		private EventHandler<UI2::NodeDestroyedEvent>,
		private EventHandler<UI2::ScreenDestructionEvent>
	{
	public:
		UIRenderPipeline_OpenGL(
			TexturedRectRenderer_OpenGL& texturedRectRenderer,
			PanelRenderer_OpenGL& panelRenderer,
			TextRenderer_OpenGL& textRenderer);
		~UIRenderPipeline_OpenGL();

		void SetScreen(UI2::Screen* newScreen);
		void Render(Vec2u targetSize);
	private:
		UI2::Screen* screen;
		TexturedRectRenderer_OpenGL& texturedRectRenderer;
		PanelRenderer_OpenGL& panelRenderer;
		TextRenderer_OpenGL& textRenderer;

		struct LayerData
		{
			TexturedRectRenderCache_OpenGL texturedRectRenderCache;
			PanelRenderCache_OpenGL panelRenderCache;
			TextRenderCache_OpenGL textRenderCache;			

			bool imagesDirty;
			UnorderedArray<UI2::Image*> images;
			bool panelsDirty;
			UnorderedArray<UI2::Panel*> panels;
			bool textsDirty;
			UnorderedArray<UI2::Text*> texts;			
		};
		List<LayerData> layers;
		bool dirty;

		struct RenderData : EventHandler<UI2::NodeTransformUpdatedEvent>
		{
			struct RenderPipelineData
			{				
				List<LayerData>::Iterator layerIt;
				uint index;
			};
			Map<UIRenderPipeline_OpenGL*, RenderPipelineData> renderPipelines;

			void OnEvent(UI2::NodeTransformUpdatedEvent event) override;
		};

		void RecreatePanelRenderData(LayerData& layer);
		void RecreateImageRenderData(LayerData& layer);
		void RecreateTextRenderData(LayerData& layer);

		List<UIRenderPipeline_OpenGL::LayerData>::Iterator GetNodeLayer(UI2::Node* node);

		void UnsubscribeNode(UI2::Node* node);
		void UnsubscribeNode(UI2::Node* node, RenderData& renderData);
		RenderData::RenderPipelineData& SubscribeNode(UI2::Node* node);

		void AddNode(UI2::Node* node);
		void RemoveNode(UI2::Node* node);

		void AddNodeTree(UI2::Node* node);
		void SetNodeRenderDataArrayIndex(UI2::Node* node, uint newIndex);

		void OnEvent(UI2::NodeCreatedEvent event) override;
		void OnEvent(UI2::NodeDestroyedEvent event) override;
		void OnEvent(UI2::ScreenDestructionEvent event) override;
	};
}