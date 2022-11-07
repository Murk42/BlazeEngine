#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIElementManager.h"

namespace Blaze
{
	namespace UI
	{
		class ImageManager;

		struct ImageProperties
		{
			UIElementProperty<Graphics::Core::Texture2D*> texture;
			UIElementProperty<Rectf> sourceRect;
			UIElementProperty<ColorRGBAf> mask;
		};

		class BLAZE_API Image : public UIElement
		{
		public:			
			ColorRGBAf mask;			
			Rectf sourceRect;
			Graphics::Core::Texture2D* texture;

			Image();
			
			void SetProperties(const ImageProperties&);

			using ManagerType = ImageManager;
			static constexpr const char* typeName = "Image";
		};

		class BLAZE_API ImageManager : public UIElementManager<Image>
		{
			Graphics::Core::VertexArray imagesVA;
			Graphics::Core::ShaderProgram imagesSP;
			Graphics::Core::GraphicsBuffer imagesVB;
		public:
			void Setup() override;

			void Render(UIElement*) override;
		};
	}
}