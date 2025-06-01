#pragma once
#include "BlazeEngineGraphics/UI/Core/Node.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnit.h"
#include "BlazeEngineGraphics/RenderScene/RenderObject.h"
#include "BlazeEngineGraphics/UI/Common/StringUTF8TextContainer.h"

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
		
		void SetText(StringViewUTF8 text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);
		void SetFontStyle(const FontStyle& fontStyle);		
		void SetLayoutOptions(TextLayoutOptions layoutOptions);
		void SetCullingNode(Node* cullingNode);		
				
		inline StringViewUTF8 GetText() const { return textContainer.GetString(); }
		inline const auto& GetCharacterData() { return renderUnit.GetCharacterData(); }
		inline const auto& GetCharacterRenderData() { return renderUnit.GetCharacterRenderData(); }
		inline const auto& GetLineData() { return renderUnit.GetLineData(); }
		inline Node* GetCullingNode() { return renderUnit.GetCullingNode(); }		

		Graphics::RenderUnit* GetRenderUnit(uint index) override;
	private:
		StringUTF8TextContainer textContainer;		
	};
}