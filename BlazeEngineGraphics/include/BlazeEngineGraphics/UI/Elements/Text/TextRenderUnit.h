#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/Text/FontAtlasesData.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze::UIGraphics
{
	enum class TextLineAlign
	{
		Left, Center, Right,
	};
	enum class TextHorizontalLayout
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
	enum class TextVerticalLayout
	{
		AlignTop,
		AlignBottom,
		AlignCenter,
		ChangeSize
	};

	class BLAZE_GRAPHICS_API TextRenderUnit :
		public TextRenderUnitBase,		
		private EventHandler<UI::Node::TransformUpdatedEvent>,
		private EventHandler<UI::Node::FinalTransformUpdatedEvent>
	{		
	public:										
		TextRenderUnit(UI::Node* node);		
			
		void BeginStream() override;
		void* Advance() override;		

		bool CleanData() override;
		bool CleanRenderData() override;

		UI::NodeFinalTransform GetFinalTransform() override;
		
		void SetText(const StringViewUTF8& text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);
		void SetFont(Font& font);
		void SetFontHeight(uint pixelFontHeight);
		void SetLayoutOptions(TextHorizontalLayout horizontalLayout, TextVerticalLayout verticalLayout, TextLineAlign lineAlign, bool multiline);
		void SetCullingNode(UI::Node* node);
		void SetLineDistance(float distance);		

		const Array<CharacterData>& GetCharacterData() override;
		const Array<CharacterRenderData>& GetCharacterRenderData() override;	
		const Array<LineData>& GetLineData() override;
		UI::NodeFinalTransform GetCullingTransform() override;
		uint GetPixelFontHeight() override { return pixelFontHeight; }
		inline UI::Node* GetNode() const { return node; }
		inline Font* GetFont() const { return font; }
		inline StringViewUTF8 GetText() const { return text; }
		inline UI::Node* GetCullingNode() const { return cullingNode; }		
		inline float GetLineDistanceCoefficient() const { return lineDistanceCoefficient; }
		inline bool IsMultiline() const { return multiline; }
		inline TextHorizontalLayout GetHorizontalLayout() const { return horizontalLayout; }
		inline TextVerticalLayout GetVerticalLayout() const { return verticalLayout; }
		inline TextLineAlign GetLineAlign() const { return lineAlign; }		
	private:
		UI::Node* node;					
		Array<CharacterData> characterData;		
		Array<CharacterRenderData> characterRenderData;
		Array<ColorRGBAf> characterColors;		
		Array<LineData> lineData;
		StringUTF8 text;
		Font* font;
		Graphics::FontAtlasesData* fontAtlasesData;
		uint pixelFontHeight;		
		UI::Node* cullingNode;		

		bool renderDataDirty : 1;
		bool dataDirty : 1;

		bool multiline : 1;
		TextLineAlign lineAlign;
		TextHorizontalLayout horizontalLayout;
		TextVerticalLayout verticalLayout;
		float lineDistanceCoefficient;

		Vec2f textSize;

		uint characterIndex;
		Graphics::TexturedRectRenderData renderData;

		void SkipCulledCharacters();								

		Array<TextLineLayoutData> GetTextLineLayoutData();
		
		void OnEvent(UI::Node::TransformUpdatedEvent event) override;
		void OnEvent(UI::Node::FinalTransformUpdatedEvent event) override;		
	};
}