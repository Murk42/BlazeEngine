#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze
{
	class TextLayouterBase;
}

namespace Blaze::UI::Nodes
{
	class Text : 
		public Node,
		public Graphics::RenderObject
	{
	public:		
		TextRenderUnit renderUnit;

		Text();
		~Text();
		
		void SetText(StringUTF8 text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);
		void SetFont(Font& font);
		void SetFontHeight(uint pixelFontHeight);
		void SetLayoutOptions(TextLayoutOptions layoutOptions);
		void SetCullingNode(Node* cullingNode);		
		
		inline Font* GetFont() const { return renderUnit.GetFont(); }
		inline StringViewUTF8 GetText() const { return renderUnit.GetText(); }
		inline uint GetPixelFontHeight() { return renderUnit.GetPixelFontHeight(); }
		inline const auto& GetCharacterData() { return renderUnit.GetCharacterData(); }
		inline const auto& GetCharacterRenderData() { return renderUnit.GetCharacterRenderData(); }
		inline const auto& GetLineData() { return renderUnit.GetLineData(); }
		inline Node* GetCullingNode() { return renderUnit.GetCullingNode(); }		

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
	};
}