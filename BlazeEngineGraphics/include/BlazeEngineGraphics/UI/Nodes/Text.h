#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"

namespace Blaze
{
	class TextLayouterBase;
}

namespace Blaze::UI
{
	using TextLayoutOptions = UIGraphics::TextLayoutOptions;
	using TextLineHorizontalAlign = UIGraphics::TextLineHorizontalAlign;
	using TextLineVerticalAlign = UIGraphics::TextLineVerticalAlign;
	using TextHorizontallyUnderfittedOptions = UIGraphics::TextHorizontallyUnderfittedOptions;
	using TextVerticallyUnderfittedOptions = UIGraphics::TextVerticallyUnderfittedOptions;
	using TextHorizontallyOverfittedOptions = UIGraphics::TextHorizontallyOverfittedOptions;
	using TextVerticallyOverfittedOptions = UIGraphics::TextVerticallyOverfittedOptions;	

	class Text : 
		public Node,
		public Graphics::RenderObject
	{
	public:		
		UIGraphics::TextRenderUnit renderUnit;
		//EventDispatcher<UIGraphics::TextRenderUnit::RenderDataUpdatedEvent>& renderDataUpdatedEventDispatcher = renderUnit.renderDataUpdatedEventDispatcher;

		Text();
		~Text();
		
		void SetText(StringUTF8 text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);
		void SetFont(Font& font);
		void SetFontHeight(uint pixelFontHeight);
		void SetLayoutOptions(TextLayoutOptions layoutOptions);
		void SetCullingNode(UI::Node* cullingNode);		
		
		inline Font* GetFont() const { return renderUnit.GetFont(); }
		inline StringViewUTF8 GetText() const { return renderUnit.GetText(); }
		inline uint GetPixelFontHeight() { return renderUnit.GetPixelFontHeight(); }
		inline const auto& GetCharacterData() { return renderUnit.GetCharacterData(); }
		inline const auto& GetCharacterRenderData() { return renderUnit.GetCharacterRenderData(); }
		inline const auto& GetLineData() { return renderUnit.GetLineData(); }
		inline UI::Node* GetCullingNode() { return renderUnit.GetCullingNode(); }		

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
	};
}