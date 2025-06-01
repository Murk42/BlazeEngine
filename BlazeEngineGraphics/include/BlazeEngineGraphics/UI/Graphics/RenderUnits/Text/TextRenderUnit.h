#pragma once
#include "BlazeEngineCore/DataStructures/Align.h"
#include "BlazeEngineGraphics/BlazeEngineGraphicsDefines.h"
#include "BlazeEngineGraphics/UI/Graphics/RenderUnits/Text/TextRenderUnitBase.h"
#include "BlazeEngineGraphics/Renderers/TexturedRectRenderer.h"
#include "BlazeEngineGraphics/UI/Common/TextContainerBase.h"

namespace Blaze::UI
{		
	enum class TextHorizontallyUnderfittedOptions : uint8
	{
		ResizeToFit, Nothing, Squish, CharacterWrap, WordWrap, WordWrapSpread
	};
	enum class TextVerticallyUnderfittedOptions : uint8
	{
		ResizeToFit, Nothing, Squish
	};	
	enum class TextHorizontallyOverfittedOptions : uint8
	{
		ResizeToFit, Nothing, SpreadCharacters, SpreadWords
	};
	enum class TextVerticallyOverfittedOptions : uint8
	{
		ResizeToFit, Nothing, SpreadLines
	};

	struct TextLayoutOptions
	{
		float lineAdvance = 1.0f;
		float wrappedLineAdvance = 0.9f;		
		HorizontalAlign lineHorizontalAlign = HorizontalAlign::Center;
		VerticalAlign lineVerticalAlign = VerticalAlign::Center;
		TextHorizontallyUnderfittedOptions horizontallyUnderfittedOption = TextHorizontallyUnderfittedOptions::Nothing;
		TextVerticallyUnderfittedOptions verticallyUnderfittedOption = TextVerticallyUnderfittedOptions::Nothing;
		TextHorizontallyOverfittedOptions horizontallyOverfittedOption = TextHorizontallyOverfittedOptions::Nothing;
		TextVerticallyOverfittedOptions verticallyOverfittedOption = TextVerticallyOverfittedOptions::Nothing;
	};		

	class BLAZE_GRAPHICS_API TextRenderUnit :
		public TextRenderUnitBase
	{		
	public:										
		TextRenderUnit(TextContainerBase& textContainer, Node* node);
		~TextRenderUnit();
			
		void BeginStream() override;
		void* Advance() override;		

		bool CleanData() override;
		bool CleanRenderData() override;

		NodeFinalTransform GetFinalTransform() override;
				
		void SetTextColor(ColorRGBAf color);
		void SetTextCharactersColor(const ArrayView<ColorRGBAf>& colors);		
		void SetFontStyle(const FontStyle& style);
		void SetLayoutOptions(TextLayoutOptions layoutOptions);		
		void SetCullingNode(Node* node);		

		const Array<CharacterData>& GetCharacterData() override;
		const Array<CharacterRenderData>& GetCharacterRenderData() override;	
		const Array<LineData>& GetLineData() override;
		NodeFinalTransform GetCullingTransform() override;		
		inline Node* GetNode() const { return node; }		
		inline const FontStyle* GetFontStyle() const override { return fontStyle; }
		inline TextContainerBase& GetTextContainer() const { return textContainer; }
		inline Node* GetCullingNode() const { return cullingNode; }				
		inline TextLayoutOptions GetLayoutOptions() const { return layoutOptions; }
	private:		
		Node* node;					
		Array<CharacterData> characterData;		
		Array<CharacterRenderData> characterRenderData;
		Array<ColorRGBAf> characterColors;		
		Array<LineData> lineData;
		
		const FontStyle* fontStyle;

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
		static float GetLinesVerticalOffset(VerticalAlign align, float textHeight, float boundingHeight);
		static float GetLineHorizontalOffset(HorizontalAlign align, float lineWidth, float boundingWidth);
		
		void TextUpdatedEvent(const TextContainerBase::TextUpdatedEvent& event);
		void TransformUpdatedEvent(const Node::TransformUpdatedEvent& event);
		void FinalTransformUpdatedEvent(const Node::FinalTransformUpdatedEvent& event);
	};
}