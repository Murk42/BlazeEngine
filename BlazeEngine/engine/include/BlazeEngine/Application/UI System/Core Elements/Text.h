#pragma once
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Graphics/Graphics.h"
#include "BlazeEngine/DataStructures/StringUTF8.h"
#include "BlazeEngine/Resources/Font/Font.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include "BlazeEngine/Application/UI System/UIManager.h"
#include "BlazeEngine/Application/UI System/UIEvent.h"

namespace Blaze
{
	namespace UI
	{
		class TextManager;

		struct TextProperties
		{
			UIElementProperty<StringUTF8> text;
			UIElementProperty<ColorRGBAf> textColor;

			UIElementProperty<FontResolution*> fontResolution;
			UIElementProperty<uint> fontSize;
		};

		class BLAZE_API Text : public UIElement
		{		
			struct Vertex
			{
				Vec2f p1;
				Vec2f p2;
				Vec2f uv1;
				Vec2f uv2;
				float next;
			};

			Graphics::Core::VertexArray va;
			Graphics::Core::GraphicsBuffer vb;
			std::vector<Vertex> vertices;
			
			FontResolution* fontResolution;
			size_t fontSize;

			Vec2f bottomLeft;
			Vec2f topRight;

			StringUTF8 text;

			void GenerateTextVertices();
		public:
			Text();
			~Text();
			
			ColorRGBAf color;
			
			void SetFontSize(size_t fontSize);
			void SetFontResolution(FontResolution* fontResolution);
			void SetText(StringUTF8 text);


			float GetBaselineDistance() const;
			inline FontResolution* GetFontResolution() const { return fontResolution; }
			inline size_t GetFontSize() const { return fontSize; }			
			inline StringUTF8 GetText() const { return text; }			
			inline const std::vector<Vertex>& GetVertices() const { return vertices; }
			
			void SetProperties(const TextProperties&);

			using ManagerType = TextManager;
			static constexpr const char* typeName = "Text";

			friend class TextManager;
		};

		class BLAZE_API TextManager : public UIElementManager<Text>
		{
			Graphics::Core::ShaderProgram programNormal;			
			Graphics::Core::ShaderProgram programSDF;
			Graphics::Core::ShaderProgram programLCD;			
			
			Graphics::Core::ShaderProgram* SelectProgram(FontResolutionRenderType);
		public:
			void Setup() override;
			
			void Render(UIElement*) override;
			void Update(UIElement*) override;
		};
	}
}