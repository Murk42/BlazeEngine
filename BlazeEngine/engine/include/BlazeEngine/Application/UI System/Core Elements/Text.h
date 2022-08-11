#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Application/UI System/UIManager.h"

namespace Blaze
{
	namespace UI
	{
		class TextManager;

		class BLAZE_API Text : public UIElement
		{		
			Graphics::Core::VertexArray va;
			Graphics::Core::GraphicsBuffer vb;
			FontResolution* fontResolution;

			uint vertexCount;

			Vec2f bottomLeft;
			Vec2f topRight;

			StringUTF8 text;			
			bool dirty;			
		public:
			Text();
			
			float fontSize;
			bool shown;
			ColorRGBAf color;
			Rectf clipRect;						

			std::function<void()> sizeChanged;
			
			void SetFontResolution(FontResolution* fontResolution);
			void SetText(StringViewUTF8 text);

			inline FontResolution* GetFontResolution() const { return fontResolution; }
			inline StringUTF8 GetText() const { return text; }			

			using ManagerType = TextManager;
			static constexpr const char* typeName = "Text";

			friend class TextManager;
		};

		class BLAZE_API TextManager : public UIElementManager<Text>
		{
			Graphics::Core::ShaderProgram programNormal;			
			Graphics::Core::ShaderProgram programSDF;
			Graphics::Core::ShaderProgram programLCD;			

			void GenerateVertices(Text& text, FontResolution*);
			Graphics::Core::ShaderProgram* SelectProgram(Text& text);
		public:
			void Setup() override;
			
			void Render(size_t index, size_t end) override;
			void Update(size_t index, size_t end) override;
		};
	}
}