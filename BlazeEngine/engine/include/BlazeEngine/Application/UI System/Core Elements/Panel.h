#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/DataStructures/Color.h"

namespace Blaze
{
	namespace UI
	{
		class PanelManager;

		class BLAZE_API Panel : public UIElement
		{
		public:
			bool shown;
			ColorRGBAf fillColor;
			ColorRGBAf borderColor;
			float borderWidth;
			float cornerRadius;			

			Panel();			
			 
			using ManagerType = PanelManager;
			static constexpr const char* typeName = "Panel";
		};

		class BLAZE_API PanelManager : public UIElementManager<Panel>
		{			
			Graphics::Core::VertexArray panelsVA;
			Graphics::Core::ShaderProgram panelsSP;
			Graphics::Core::GraphicsBuffer panelsVB;
		public:
			void Setup() override;
			
			void Render(size_t index, size_t end) override;
		};
	}
}