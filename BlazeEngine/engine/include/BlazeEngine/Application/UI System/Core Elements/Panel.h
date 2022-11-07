#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
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

		struct PanelProperties
		{
			UIElementProperty<float> borderWidth;
			UIElementProperty<float> cornerRadius;
			UIElementProperty<ColorRGBAf> fillColor;
			UIElementProperty<ColorRGBAf> borderColor;
		};

		class BLAZE_API Panel : public UIElement
		{
		public:			
			ColorRGBAf fillColor;
			ColorRGBAf borderColor;
			float borderWidth;
			float cornerRadius;			

			Panel();
			~Panel();
			
			void SetProperties(const PanelProperties&);
			 
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
			
			void Render(UIElement*) override;
		};
	}
}