#pragma once
#include "BlazeEngineGraphics/UI/Elements/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/Text/FontAtlasesData.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze::UIGraphics
{
	enum class TextLineHorizontalAlign
	{
		Left, Center, Right,
	};

	enum class TextLineVerticalAlign
	{
		Top, Center, Bottom
	};

	enum class TextHorizontallyUnderfittedOptions
	{
		ResizeToFit, Nothing, Squish, CharacterWrap, WordWrap,
	};

	enum class TextVerticallyUnderfittedOptions
	{
		ResizeToFit, Nothing, Squish
	};
	
	enum class TextHorizontallyOverfittedOptions
	{
		ResizeToFit, Nothing, SpreadCharacters, SpreadWords
	};

	enum class TextVerticallyOverfittedOptions
	{
		ResizeToFit, Nothing, SpreadLines
	};

	struct TextLayoutOptions
	{
		TextLineHorizontalAlign lineHorizontalAlign = TextLineHorizontalAlign::Center;
		TextLineVerticalAlign lineVerticalAlign = TextLineVerticalAlign::Center;
		TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption = TextHorizontallyUnderfittedOptions::Nothing;
		TextVerticallyUnderfittedOptions verticallyUnderfittedOption = TextVerticallyUnderfittedOptions::Nothing;
		TextHorizontallyOverfittedOptions horizontallyOverfittedOption = TextHorizontallyOverfittedOptions::Nothing;
		TextVerticallyOverfittedOptions verticallyOverfittedOption = TextVerticallyOverfittedOptions::Nothing;
		float lineAdvance = 1.0f;
		float wrappedLineAdvance = 0.9f;
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
		void SetLayoutOptions(TextLayoutOptions layoutOptions);		
		void SetCullingNode(UI::Node* node);		

		const Array<CharacterData>& GetCharacterData() override;
		const Array<CharacterRenderData>& GetCharacterRenderData() override;	
		const Array<LineData>& GetLineData() override;
		UI::NodeFinalTransform GetCullingTransform() override;
		uint GetPixelFontHeight() override { return pixelFontHeight; }
		inline UI::Node* GetNode() const { return node; }
		inline Font* GetFont() const { return font; }
		inline StringViewUTF8 GetText() const { return text; }
		inline UI::Node* GetCullingNode() const { return cullingNode; }				
		inline TextLayoutOptions GetLayoutOptions() const { return layoutOptions; }
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
		TextLayoutOptions layoutOptions;		

		Vec2f textSize;

		bool renderDataDirty : 1;
		bool dataDirty : 1;

		uint characterIndex;
		Graphics::TexturedRectRenderData renderData;

		void SkipCulledCharacters();								

		static Array<TextLineLayoutData> GetLineLayoutData(StringViewUTF8 text, Font* font, uint pixelFontHeight, float lineAdvance);
		static Vec2f FitLinesIntoBounds(
			TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption,
			TextHorizontallyOverfittedOptions horizontallyOverfittedOption,
			TextVerticallyUnderfittedOptions verticallyUnderfittedOption,
			TextVerticallyOverfittedOptions verticallyOverfittedOption, 
			Array<TextLineLayoutData>& lines, float wrappedLineAdvance, Vec2f& boundingSize
		);
		static float GetLinesVerticalOffset(TextLineVerticalAlign align, float textHeight, float boundingHeight);
		static float GetLineHorizontalOffset(TextLineHorizontalAlign align, float lineWidth, float boundingWidth);
		
		void OnEvent(UI::Node::TransformUpdatedEvent event) override;
		void OnEvent(UI::Node::FinalTransformUpdatedEvent event) override;		
	};
}