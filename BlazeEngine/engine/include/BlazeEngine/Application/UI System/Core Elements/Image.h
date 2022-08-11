#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Color.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"
#include "BlazeEngine/Application/UI System/UIElementManager.h"

namespace Blaze
{
	namespace UI
	{
		class ImageManager;

		class BLAZE_API Image : public UIElement
		{
		public:
			bool shown;
			ColorRGBAf mask;			
			Rectf sourceRect;
			Graphics::Core::Texture2D* texture;

			Image();			

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

			void Render(size_t index, size_t end) override;						
		};
	}
}