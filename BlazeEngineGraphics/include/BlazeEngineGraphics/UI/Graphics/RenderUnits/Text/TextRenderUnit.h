#pragma once
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/Text/FontAtlasesData.h"
#include "BlazeEngine/Resources/Font/TextLayouter.h"

namespace Blaze::UI
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
		ResizeToFit, Nothing, Squish, CharacterWrap, WordWrap, WordWrapSpread
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
		private EventHandler<Node::TransformUpdatedEvent>,
		private EventHandler<Node::FinalTransformUpdatedEvent>
	{		
	public:										
		TextRenderUnit(Node* node);	
		~TextRenderUnit();
			
		void BeginStream() override;
		void* Advance() override;		

		bool CleanData() override;
		bool CleanRenderData() override;

		NodeFinalTransform GetFinalTransform() override;
		
		void SetText(const StringViewUTF8& text);
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);
		void SetFont(Font& font);
		void SetFontHeight(uint pixelFontHeight);
		void SetLayoutOptions(TextLayoutOptions layoutOptions);		
		void SetCullingNode(Node* node);		

		const Array<CharacterData>& GetCharacterData() override;
		const Array<CharacterRenderData>& GetCharacterRenderData() override;	
		const Array<LineData>& GetLineData() override;
		NodeFinalTransform GetCullingTransform() override;
		uint GetPixelFontHeight() override { return pixelFontHeight; }
		inline Node* GetNode() const { return node; }
		inline Font* GetFont() const { return font; }
		inline StringViewUTF8 GetText() const { return text; }
		inline Node* GetCullingNode() const { return cullingNode; }				
		inline TextLayoutOptions GetLayoutOptions() const { return layoutOptions; }
	private:
		Node* node;					
		Array<CharacterData> characterData;		
		Array<CharacterRenderData> characterRenderData;
		Array<ColorRGBAf> characterColors;		
		Array<LineData> lineData;
		StringUTF8 text;
		Font* font;
		Graphics::FontAtlasesData* fontAtlasesData;
		uint pixelFontHeight;		
		Node* cullingNode;		
		TextLayoutOptions layoutOptions;				

		bool renderDataDirty : 1;
		bool dataDirty : 1;

		uint characterIndex;
		Graphics::TexturedRectRenderData renderData;

		void SkipCulledCharacters();								

		struct TempCharacterData
		{
			Vec2f offset;
			Vec2f size;
			float advance;
			UnicodeChar character;
		};
		
		Array<TempCharacterData> GetTempCharacterData();
		void InitializeLineAndCharacterData();
		static float GetTextBoundingWidth(Array<LineData>& lineData);
		static float GetTextBoundingHeight(Array<LineData>& lineData);				
		static void ManageHorizontalUnderfittedOptions(TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption, float wrappedLineAdvance, float& textSize, float& boundingWidth, Array<LineData>& lineData, Array<CharacterData>& characterData);
		static void ManageHorizontalOverfittedOptions(TextHorizontallyOverfittedOptions horizontallyOverfittedOption, float& textSize, float& boundingWidth, Array<LineData>& lineData, Array<CharacterData>& characterData);				
		static void ManageVerticalUnderfittedOptions(TextVerticallyUnderfittedOptions verticallyUnderfittedOption, float& textHeight, float& boundingHeight, Array<LineData>& lineData, Array<CharacterData>& characterData);
		static void ManageVerticalOverfittedOptions(TextVerticallyOverfittedOptions verticallyOverfittedOption, float& textHeight, float& boundingHeight, Array<LineData>& lineData, Array<CharacterData>& characterData);
		static float GetLinesVerticalOffset(TextLineVerticalAlign align, float textHeight, float boundingHeight);
		static float GetLineHorizontalOffset(TextLineHorizontalAlign align, float lineWidth, float boundingWidth);
		
		void OnEvent(const Node::TransformUpdatedEvent& event) override;
		void OnEvent(const Node::FinalTransformUpdatedEvent& event) override;		
	};
}