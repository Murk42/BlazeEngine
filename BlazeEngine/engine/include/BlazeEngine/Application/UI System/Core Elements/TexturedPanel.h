#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Graphics.h"

namespace Blaze
{
	namespace UI
	{
		class TexturedPanelManager;

		struct TexturedPanelSpacing
		{
			float left;
			float right;
			float top;
			float bottom;
		};

		struct TexturedPanelProperties
		{
			UIElementProperty<Graphics::Core::Texture2D*> texture;
			UIElementProperty<ColorRGBAf> mask;
			UIElementProperty<Rectf> sourceRect;
			UIElementProperty<TexturedPanelSpacing> textureSpacing;
			UIElementProperty<TexturedPanelSpacing> spacing;
		};

		class BLAZE_API TexturedPanel : public UIElement
		{
		public:			
			
			Graphics::Core::Texture2D* texture;
			ColorRGBAf mask;
			Rectf sourceRect;
			TexturedPanelSpacing textureSpacing;
			TexturedPanelSpacing spacing;

			TexturedPanel();

			void SetProperties(const TexturedPanelProperties& p);

			using ManagerType = TexturedPanelManager;
			static constexpr const char* typeName = "TexturedPanel";
		};

		class BLAZE_API TexturedPanelManager : public UIElementManager<TexturedPanel>
		{
			Graphics::Core::GraphicsBuffer VB;
			Graphics::Core::VertexArray VA;
			Graphics::Core::ShaderProgram SP;
		public:
			TexturedPanelManager();

			void Setup() override;

			void Render(UIElement*) override;
		};
	}
}