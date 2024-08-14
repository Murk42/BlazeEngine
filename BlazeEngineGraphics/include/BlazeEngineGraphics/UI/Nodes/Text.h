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
	using TextHorizontalLayout = UIGraphics::TextHorizontalLayout;
	using TextVerticalLayout = UIGraphics::TextVerticalLayout;
	using TextLineAlign = UIGraphics::TextLineAlign;	

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
		void SetLayoutOptions(TextHorizontalLayout horizontalLayout, TextVerticalLayout verticalLayout, TextLineAlign lineAlign, bool multiline);
		void SetCullingNode(UI::Node* cullingNode);
		void SetLineDistance(float distance);
		
		inline Font* GetFont() const { return renderUnit.GetFont(); }
		inline StringViewUTF8 GetText() const { return renderUnit.GetText(); }
		inline uint GetPixelFontHeight() { return renderUnit.GetPixelFontHeight(); }
		inline auto GetCharacterData() { return renderUnit.GetCharacterData(); }
		inline auto GetCharacterRenderData() { return renderUnit.GetCharacterRenderData(); }
		inline auto GetLineData() { return renderUnit.GetLineData(); }
		inline UI::Node* GetCullingNode() { return renderUnit.GetCullingNode(); }
		inline float GetLineDistanceCoefficient() const { return renderUnit.GetLineDistanceCoefficient(); }

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
	};
}