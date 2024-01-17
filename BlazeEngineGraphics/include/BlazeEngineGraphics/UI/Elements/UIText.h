#pragma once
#include "BlazeEngineGraphics/UI/Core/UINode.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderStreams/OpenGL/TextRenderStream_OpenGL.h"

namespace Blaze
{
	class TextLayouterBase;
}

namespace Blaze::UI
{
	class Text : public Node, private EventHandler<NodePreTransformUpdatedEvent>
	{
	public:
		enum class LineAlign
		{
			Left, Center, Right,
		};
		enum class HorizontalLayout
		{						
			//Text line will be squished if it exceeds the width of the node			
			Squish,

			//multi-line: Text will be wrapped if it exceeds the width of the node,
			//single-line: Characters that exceed the width of the node aren't shown
			Wrap,			

			//Text will go outside the node if it exceeds the width of the node			
			NoWrap,

			//The node size will change according to text size
			//Line alignment has no effect
			ChangeSize,
		};
		enum class VerticalLayout
		{
			AlignTop,
			AlignBottom,
			AlignCenter,
			ChangeSize
		};

		struct CharacterData
		{
			UnicodeChar character;
			ColorRGBAf color;
			Vec2f pos;
			Vec2f size;
		};

		Node* cullingNode;

		Text(Node* parent, const NodeTransform& transform);
		~Text();
		
		void SetText(StringUTF8 text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(Array<ColorRGBAf> colors);
		void SetFont(Font& font);
		
		void SetFontHeight(uint pixelFontHeight);

		void SetLayoutOptions(HorizontalLayout horizontalLayout, VerticalLayout verticalLayout, LineAlign lineAlign, bool multiline);

		inline String GetTypeName() const { return "Text"; }
		inline Font* GetFont() const { return font; }
		inline StringViewUTF8 GetText() const { return text; }
		inline uint GetPixelFontHeight() const { return pixelFontHeight; }
		inline ArrayView<CharacterData> GetCharacterData() const { return characterData; };
	private:
		Graphics::OpenGL::TextRenderStream_OpenGL renderStream;
		
		StringUTF8 text;
		ColorRGBAf textColor;
		Array<ColorRGBAf> charactersColors;
		Font* font;
		
		bool characterDataDirty;
		Array<CharacterData> characterData;		
		uint pixelFontHeight;				

		bool multiline;
		LineAlign lineAlign;
		HorizontalLayout horizontalLayout;
		VerticalLayout verticalLayout;

		Vec2f textSize;

		void GenerateCharacterData();				
		void CopyCharacterDataFromLayouter(TextLayouterBase* layouter);
		
		void OnEvent(NodePreTransformUpdatedEvent event) override;
	};
}