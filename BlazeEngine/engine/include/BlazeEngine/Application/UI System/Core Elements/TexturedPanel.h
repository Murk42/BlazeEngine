#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Graphics.h"

namespace Blaze
{
	namespace UI
	{
		class TexturedPanelManager;

		class BLAZE_API TexturedPanel : public UIElement
		{
		public:
			struct Dimensions
			{
				int left;
				int right;
				int top;
				int bottom;
			};

			bool shown;
			ColorRGBAf mask;			
			Rectf sourceRect;
			Graphics::Core::Texture2D* texture;
			Dimensions dimensions;

			TexturedPanel();			

			using ManagerType = TexturedPanelManager;
			static constexpr const char* typeName = "TexturedPanel";
		};

		class BLAZE_API TexturedPanelManager : public UIElementManager<TexturedPanel>
		{
			Graphics::Core::VertexArray texturedPanelsVA;
			Graphics::Core::ShaderProgram texturedPanelsSP;
		public:
			void Setup() override;

			void Render(size_t index, size_t end) override;
		};
	}
}